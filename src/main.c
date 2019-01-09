#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "parser.tab.h"
#include "shell.h"

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int
yyparse();
extern YY_BUFFER_STATE
yy_scan_string(char * str);
extern void
yylex_destroy();
extern char * optarg;
extern int optind;
extern int EXIT_CODE;


int
main(int argc, char * argv[])
{
	struct {
		char * command;
	} opts;
	int opt;
	opts.command == NULL;
	int optlen;
	while ((opt = getopt(argc, argv, "abc:")) != -1)
		switch (opt) {
			case 'c':
				optlen = strlen(optarg);
				opts.command = malloc(optlen + 1);
				strncpy(opts.command, optarg, optlen);
				opts.command[optlen] = '\0';
				break;
			case '?':
				fprintf(stderr,
				    "usage: %s [-c command | filename]\n",
				    basename(argv[0]));
				break;
		}

	sh_init();

	if (opts.command != NULL) {
		yy_scan_string(opts.command);
		yyparse();
		yylex_destroy();
		free(opts.command);
	} else if (optind < argc)    {
		int fd = open(argv[optind], O_RDONLY);
		if (fd == -1) {
			if (errno == EACCES) {
				fprintf(stderr, "Unable to find or open "
				"specified file.\n");
			} else   {
				fprintf(stderr, "Unhandled exception while "
				"opening the file.\n");
			}
			EXIT_CODE = 1;
			sh_exit();
		}
		int buff_size = 4096;
		char * buff = malloc(buff_size);
		char * buff_loc = buff;
		ssize_t buff_read;
		while ((buff_read = read(fd, buff_loc, buff_size -
		    (buff_loc - buff)))) {
			if (buff_read == -1) {
				fprintf(stderr, "Unable to read from file.\n");
				free(buff);
				close(fd);
				EXIT_CODE = 1;
				sh_exit();
			}
			if (buff_read == buff_size - (buff_loc - buff)) {
				int offset = buff_loc - buff;
				buff_size *= 2;
				buff = realloc(buff, buff_size);
				buff_loc = buff + offset;
			}
		}
		yy_scan_string(buff);
		yyparse();
		yylex_destroy();
		free(buff);
		close(fd);
	} else   {
		char * line = readline(sh_getprompt());
		while (line != NULL) {
			yy_scan_string(line);
			yyparse();
			yylex_destroy();
			free(line);
			line = readline(sh_getprompt());
		}
		printf("\n");
	}
	sh_exit();
} /* main */
