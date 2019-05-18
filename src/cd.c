#include "cd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "shell.h"

extern int EXIT_CODE;
extern int errno;

char * PWD    = NULL;
char * OLDPWD = NULL;

int
cd_iscd(char * command)
{
	return (strcmp(command, "cd") == 0 ? 1 : 0);
}

void
cd_handle(int argc, char * argv[])
{
	if (argc == 1) {
		char * home = getenv("HOME");
		if (home == NULL) {
			fprintf(stderr, "This user has not specified home "
			    "directory.\n");
			EXIT_CODE = 1;
			return;
		}
		char ** args = malloc(sizeof (char *) * 3);
		args[0] = argv[0];
		args[1] = strdup(home);
		args[2] = NULL;
		cd_handle(2, args);
		free(args[1]);
		free(args);
		return;
	}
	if (argc != 2) {
		fprintf(stderr, "usage: cd [path | -]");
		EXIT_CODE = 1;
		return;
	}
	if (strcmp(argv[1], "-") == 0) {
		if (OLDPWD == NULL) {
			char * oldpwd = getenv("OLDPWD");
			if (oldpwd == NULL) {
				fprintf(stderr, "Unable to get previous "
				    "directory location.\n");
				EXIT_CODE = 1;
				return;
			}
			OLDPWD = strdup(oldpwd);
		}
		char ** args = malloc(sizeof (char *) * 3);
		args[0] = argv[0];
		args[1] = OLDPWD;
		args[2] = NULL;
		cd_handle(2, args);
		free(args);
		return;
	}
	if (PWD == NULL) {
		if (cd_init() == 0) {
			return;
		}
	}
	if (chdir(argv[1]) == -1) {
		switch (errno) {
			case EACCES:
				fprintf(stderr, "%s: Search permission is "
				    "denied for one of the components of "
				    "path.\n", argv[0]);
				break;
			case ENOENT:
				fprintf(stderr, "%s: The directory specified "
				    "in path does not exist.\n", argv[0]);
				break;
			case ENOTDIR:
				fprintf(stderr, "%s: A component of path is "
				    "not a directory.\n", argv[0]);
				break;
			default:
				fprintf(stderr, "%s: Unknown error while "
				    "changing directory.\n", argv[0]);
		}
		EXIT_CODE = 1;
		return;
	}
	if (setenv("OLDPWD", PWD, 1) != 0) {
		fprintf(stderr, "Unable to change OLDPWD variable.\n");
		EXIT_CODE = 1;
		return;
	}
	if (OLDPWD != NULL) {
		free(OLDPWD);
	}
	OLDPWD = strdup(PWD);
	if (getcwd(PWD, 4096) == NULL) {
		free(PWD);
		PWD = NULL;
		fprintf(stderr, "Unable to get current directory location.\n");
		EXIT_CODE = 1;
		return;
	}
	if (setenv("PWD", PWD, 1) != 0) {
		fprintf(stderr, "Unable to change PWD variable.\n");
		EXIT_CODE = 1;
		return;
	}
} /* cd_handle */

void
cd_cleanup()
{
	if (PWD != NULL)
		free(PWD);
	if (OLDPWD != NULL)
		free(OLDPWD);
}

int
cd_init()
{
	PWD = malloc(4096);
	if (getcwd(PWD, 4096) == NULL) {
		free(PWD);
		PWD = NULL;
		fprintf(stderr, "Unable to get current directory location.\n");
		EXIT_CODE = 1;
		return (0);
	}
	return (1);
}
