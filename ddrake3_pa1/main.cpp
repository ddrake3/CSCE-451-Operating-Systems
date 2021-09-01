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
  * Date: 02/21/2021
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

#include "command.hpp"
#include "parser.hpp"

#define NUM_COMMANDS_INTERACTIVE 25

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

void run(std::vector<shell_command> shell_commands)
{
    int execute = 1;
    for (shell_command shell: shell_commands) {
      pid_t cpid = fork();

      if(cpid < 0) {
        fprintf(stderr, "Fork Failed\n");
        exit(1);
      }
      else if (cpid == 0) {

        // check for input redirection
        istream_mode cin_mode = shell.cin_mode;
        if(cin_mode == istream_mode::file) {
          // get file name
          std::string cin_file = shell.cin_file;
          const char *ccin_file = cin_file.c_str();

          // open file for reading or create new file
          int file_desc = open(ccin_file, O_RDONLY );

          // overwrite the stdin with the newly opened file
          dup2(file_desc, 0);
        }

        // check for output redirection
        ostream_mode cout_mode = shell.cout_mode;
        if(cout_mode == ostream_mode::file) {
          // get file name
          std::string cout_file = shell.cout_file;
          const char *ccout_file = cout_file.c_str();

          // open file for reading or create new file
          int file_desc = open(ccout_file, O_CREAT | O_RDWR, 0644);

          // overwrite the stdout with the newly opened file
          dup2(file_desc, 1);


        }
        else if(cout_mode == ostream_mode::append) {

          // get file name
          std::string cout_file = shell.cout_file;
          const char *ccout_file = cout_file.c_str();

          // open file for reading or create new file
          int file_desc = open(ccout_file, O_APPEND);

          // overwrite the stdout with the newly opened file
          dup2(file_desc, 1);
        }

        if(execute) {
          // execute command
          exec(shell.cmd, shell.args);
          exit(1);
        }


      }
      else {
        int status;
        waitpid(cpid, &status, 0);

        // check logical operators and exit status of child to set execute flag
        next_command_mode next_mode = shell.next_mode;

        // must have a previous successfull command to move onto next command
        if(next_mode == next_command_mode::on_success) {
          if(WEXITSTATUS(status)) {
            execute = 0;
          }
          else {
            execute = 1;
          }
        }
        // must have a previous failed command to move onto next command
        else if(next_mode == next_command_mode::on_fail) {
          // check status code, must be != 0
          if(!WEXITSTATUS(status)) {
            execute = 0;
          }
          else {
            execute = 1;
          }
        }
      }
    }
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
