#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "command.h"
#include "shell.h"
#include "cd.h"

extern int EXIT_CODE;

pid_t RUNNING = 0;

void
comm_handle(char * command, int argc, char * argv[], int * pd,
    char ** redirections)
{
	if (!comm_handle_internal(command, argc, argv)) {
		int status;
		switch (RUNNING = fork()) {
			case -1:
				fprintf(stderr, "There were problem while "
				    "forking this process.\n");
				EXIT_CODE = 1;
				break;
			case 0:
				if (pd[0]) {
					close(0);
					dup(pd[0]);
					close(pd[0]);
				}
				if (pd[1]) {
					close(1);
					dup(pd[1]);
					close(pd[1]);
				}
				if (redirections[0] != NULL) {
					close(0);
					open(redirections[0], O_RDONLY);
				}
				if (redirections[1] != NULL) {
					close(1);
					open(redirections[1], O_WRONLY | O_CREAT
					    | O_TRUNC, 0666);
				}
				if (redirections[2] != NULL) {
					close(1);
					open(redirections[2], O_WRONLY | O_CREAT
					    | O_APPEND, 0666);
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
				break;
			default:
				if (waitpid(RUNNING, &status, 0) == -1) {
					if (errno == ECHILD) {
						fprintf(stderr, "The specified "
						    "process does not "
						    "exist.\n");
					} else if (errno == EINTR) {
						fprintf(stderr, "An unblocked "
						    "signal was cought.\n");
					} else if (errno == EINVAL) {
						fprintf(stderr, "The options "
						    "argument was invalid.\n");
					}
				}
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
comm_handle_internal(char * command, int argc, char * argv[])
{
	if (cd_iscd(command)) {
		cd_handle(argc, argv);
		return (1);
	} else if (sh_isexit(command)) {
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
