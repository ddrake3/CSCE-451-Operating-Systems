/*
 * Copyright (c) 2018, Yutaka Tsutano
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

 /***************************************************************************************
  * Author: Derek Drake
  * Date: 03/14/2021
  * CSCE-451
  *
  * This is a program will:
  * 1. read in and parse the input string
  * 2. fork a child process and execute for simple commands - this includes:
  *     a. extracting a simple command from the data structure (executable name and arguments)
  *     b. creating a new process for the executable
  *     c. supplying correct arguments to the newly created process
  *     d. wait for the process to cpmlete, collect exit code
  *     e. output the result to console (for now... later we'll modify this to handle a redirection operator)
  * 3. handle logical operators - this includes:
  *     a. check if current and previous command are separated by a logical operator
  *     b. if it does, get the exit status of the previous command
  *     c. based on the exit status and logical operator, determine whether you need to execute current command or skip it
  *     d. if you need to skip it, set the exit status of the current command to be the same as the previous command
  * 4. implement -t swtich to change between interactive mode and grade mode
  *
  ***************************************************************************************/

  /***************************************************************************************
  *    The following source code is adapted from:
  *
  *    Title: ytsutano source code & exec function
  *    Author: Yutaka Tsutano
  *    Date: Feb 5, 2018
  *    Availability: https://github.com/ytsutano/osh-parser
  *
  *    AND:
  *    Title: Prof. Bradley's live help demo for PA1
  *    Author: Justin Bradley
  *    Date: Feb 18, 2021
  *    Availability: https://www.youtube.com/watch?t=231&v=7KxptmidqSg&feature=youtu.be
  *
  ***************************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "command.hpp"
#include "parser.hpp"

#define NUM_COMMANDS_INTERACTIVE 25
ostream_mode previousPipe;

int exec(const std::string& cmd, const std::vector<std::string>& args)
{
    // Make an ugly C-style args array.
    std::vector<char*> c_args = {const_cast<char*>(cmd.c_str())};
    for (const auto& a : args) {
        c_args.push_back(const_cast<char*>(a.c_str()));
    }
    c_args.push_back(nullptr);

    return execvp(cmd.c_str(), c_args.data());
}

int run(std::vector<shell_command> shell_commands)
{
    previousPipe = ostream_mode::term;
    int checkStatus = 0;
    int status = 0;
    next_command_mode prevMode = next_command_mode::always;
    int pipe1 = 0;
    int pipe2 = 0;

    for (shell_command shell: shell_commands) {

      // get current command with argument
      std::string command = shell.cmd;
      std::vector<std::string> argument = shell.args;

      // get input & output modes
      istream_mode inputMode = shell.cin_mode;
      ostream_mode outputMode = shell.cout_mode;

      // get file inpute & output locations
      const char *fileIn = shell.cin_file.c_str();
      const char *fileOut = shell.cout_file.c_str();

      // get the next mode
      next_command_mode nextMode = shell.next_mode;

      // check for bad exit status code
      if((checkStatus == 0 && prevMode == next_command_mode::on_fail) || (checkStatus != 0 && prevMode == next_command_mode::on_success)) {
        status = checkStatus;
        break;
      }

      // create pipe
      int  pipes[2];
      if ( pipe(pipes) == -1 ) {
        fprintf(stderr, "Pipe Failed\n");
        exit(1);
      }

      // create child process
      pid_t cpid = fork();
      if(cpid < 0) {
        fprintf(stderr, "Fork Failed\n");
        exit(1);
      }
      else if (cpid == 0) {

        // check for input redirection
        if(inputMode == istream_mode::file) {
          // open file for reading or create new file
          int file_desc = open(fileIn, O_RDONLY, 0700 );

          // overwrite the stdin with the newly opened file
          dup2(file_desc, 0);
          close(file_desc);
        }

        // check for output redirection
        if(outputMode == ostream_mode::file) {
          // open file for reading or create new file
          int file_desc = open(fileOut, O_WRONLY | O_CREAT, 0700);

          // overwrite the stdout with the newly opened file
          dup2(file_desc, 1);
          close(file_desc);
        }
        if(outputMode == ostream_mode::append) {
          // open file for reading or create new file
          int file_desc = open(fileOut, O_APPEND | O_CREAT| O_RDWR, 0700);

          // overwrite the stdout with the newly opened file
          dup2(file_desc, 1);
          close(file_desc);
        }

        // check check for pipes
        // check for input pipe
        if(inputMode == istream_mode::pipe) {
          dup2(pipe1, 0);
          close(pipes[0]);
        }
        // check for output pipe
        if(outputMode == ostream_mode::pipe) {
          previousPipe = ostream_mode::pipe;
          dup2(pipes[1], 1);
          close(pipes[1]);
        }
        // execute command
        exec(command, argument);

      }
      else {
        wait(&status);
        pipe1 = pipes[0];
        pipe2 = pipes[1];

        // must have a previous successfull command to move onto next command
        if ( outputMode != ostream_mode::term ){
          dup2(1, pipe2);
        }
        // must have a previous failed command to move onto next command
        if ( nextMode == next_command_mode::on_success || nextMode == next_command_mode::on_fail ){
          prevMode = nextMode;
          checkStatus = status;
        }
      }
    }
    return 0;
}

int main (int argc, char *argv[])
{
    std::string input_line;
    std::vector<shell_command> shell_commands;

    if (argc > 1 && argv[1] == std::string("-t")) { // -t option

      while (std::getline(std::cin, input_line)) {
        if(input_line == "exit") {
          exit(0);
        }
        else {
          try {
            shell_commands = parse_command_string(input_line);

            run(shell_commands);
          }
          catch (const std::runtime_error& e) {
              std::cout << e.what() << std::endl;
          }
        }
      }
  }

    else {

      for (int should_run = 0; should_run < NUM_COMMANDS_INTERACTIVE; should_run++) {
          // Print the prompt.
          std::cout << "osh> " << std::flush;

          // Read a single line.
          if (!std::getline(std::cin, input_line) || input_line == "exit") {
              break;
          }

          try {
            shell_commands = parse_command_string(input_line);
            run(shell_commands);
          }
          catch (const std::runtime_error& e) {
              std::cout << "osh: " << e.what() << std::endl;
          }
      }
    }
    return 0;
}
