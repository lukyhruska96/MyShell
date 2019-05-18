#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "command.h"
#include "shell.h"
#include "cd.h"

extern int EXIT_CODE;

pid_t RUNNING = 0;

void
comm_handle(char * command, int argc, char * argv[], int * pd)
{
	if (!comm_is_known(command, argc, argv)) {
		int status;
		switch (RUNNING = fork()) {
			case -1:
				fprintf(stderr, "There were problem while "
				    "forking this process.\n");
				EXIT_CODE = 1;
				break;
			case 0:
				if(pd[0]) {
					close(0); dup(pd[0]); close(pd[0]);
				}
				if(pd[1]) {
					close(1); dup(pd[1]); close(pd[1]);
				}
				execvp(command, argv);
				switch (errno) {
					case EACCES:
						fprintf(stderr, "%s: Permission"
						    " denied\n", command);
						break;
					default:
						fprintf(stderr, "%s: No such "
						    "file or directory\n",
						    command);
						break;
				}
				EXIT_CODE = 127;
				sh_exit();
			default:
				waitpid(RUNNING, &status, 0);
				RUNNING = 0;
				if (WIFEXITED(status)) {
					EXIT_CODE = WEXITSTATUS(status);
				} else if (WIFSIGNALED(status)) {
					EXIT_CODE = 128 + WTERMSIG(status);
				} else {
					fprintf(stderr,
					    "%s: Unexpected error.\n", command);
					EXIT_CODE = 1;
				}
		}
	}

	comm_cleancomm(command, argc, argv);
} /* comm_handle */

int
comm_is_known(char * command, int argc, char * argv[])
{
	if (cd_iscd(command)) {
		cd_handle(argc, argv);
		return (1);
	} else if (sh_isexit(command))    {
		comm_cleancomm(command, argc, argv);
		sh_exit();
	}
	return (0);
}

void
comm_cleancomm(char * command, int argc, char * argv[])
{
	for (int i = 1; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);
	free(command);
}

void
comm_cleanup() { }
