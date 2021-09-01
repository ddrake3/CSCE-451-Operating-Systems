#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define READ_END 0
#define WRITE_END 1

using namespace std;

int main(int argc, char ** argv)
{
// fork 2 children, one ls, one grep

		int fdp[2];
		int status = 0;
		pipe(fdp);

		int pid = fork();

		if (pid == 0) { // child process going to execute ls

				const char **args = new const char * [4];
				args[0] = "ls";
				args[1] = "-l";
				args[3] = NULL;

				// close the read end
				close(fdp[READ_END]);
				// use the write end of the pipe
				dup2(fdp[WRITE_END],STDOUT_FILENO);
				// close up the write end
				close(fdp[WRITE_END]);

				execvp("ls", (char **)args);

				exit(0);
		}
		else // parent
		{

				// close the write end of the pipe after first child is finished
				// this closes it for the new child
				close(fdp[WRITE_END]);

				// at this point only the read end of the pipe is open in the parent

				const char **args = new const char * [4];
				args[0] = "grep";
				args[1] = "pipes";
				//args[2] = "parser.cpp";
				args[3] = NULL;

				// fork grep now
				int pid = fork();


				if (pid == 0) { // child process

						// use the read end of the pipe for grep
						dup2(fdp[READ_END],STDIN_FILENO);
						// close read end
						close(fdp[READ_END]);

						execvp("grep", (char **)args);

						exit(0);
				}

				int waitforpid = wait(&status); // wait for a child
				//cout << "waited on " << waitforpid << endl;


				if (WEXITSTATUS(status) != 0)
				{
						cout << "bad 1" << endl;
				}

				waitforpid = wait(&status); // wait for the other child
				//cout << "waited on " << waitforpid << endl;


				if (WEXITSTATUS(status) != 0)
				{
						cout << "bad 2" << endl;
				}

		}
}
