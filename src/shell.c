#include "shell.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "command.h"
#include "cd.h"

int EXIT_CODE = 0;
char * SH_PROMPT;

extern char * PWD;
extern pid_t RUNNING;
extern void
yylex_destroy();


void
sh_exit_signal(int signum);

void
sh_init()
{
	signal(SIGINT, sh_exit_signal);
	SH_PROMPT = malloc(4096);
	strcpy(SH_PROMPT, "?$>");
}

void
sh_cleanup()
{
	free(SH_PROMPT);
	cd_cleanup();
}

int
sh_isexit(char * command)
{
	return (strcmp(command, "exit") == 0 ? 1 : 0);
}

void
sh_exit()
{
	sh_cleanup();
	yylex_destroy();
	exit(EXIT_CODE);
}

void
sh_exit_signal(int signum)
{
	if (RUNNING > 0) {
		kill(RUNNING, signum);
		EXIT_CODE = 128 + signum;
		fprintf(stderr, "Killed by signal %d.\n", signum);
	} else {
		fprintf(stdout, "\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

char *
sh_getprompt()
{
	if (PWD == NULL) {
		if (cd_init() != 0) {
			sprintf(SH_PROMPT, "mybash:%s$ ", PWD);
		}
	} else {
		sprintf(SH_PROMPT, "mybash:%s$ ", PWD);
	}
	return (SH_PROMPT);
}
