%{
	#include <stdio.h>
	#include <sys/queue.h>
	#include <stdlib.h>
	#include <string.h>
	#include <libgen.h>
	#include "command.h"
	#include "command.h"

	// Declare stuff from Flex that Bison needs to know about:
	extern int yylex();
	void yyerror(const char *s);
	extern int line;
	extern int EXIT_CODE;

	SLIST_HEAD(toklisth_s, tokentry_s);
	SLIST_HEAD(commlisth_s, commentry_s);

	struct toklist_s {
		int size;
		struct toklisth_s head;
	};

	struct tokentry_s {
		SLIST_ENTRY(tokentry_s) entries;
		char *token;
	};

	struct commlist_s {
		int size;
		struct commlisth_s head;
	};

	struct commentry_s {
		SLIST_ENTRY(commentry_s) entries;
		struct command_s *command;
	};

	struct command_s {
		char *command;
		int argc;
		char **argv;
	};
%}

%error-verbose

%union {
	char *token;
	struct command_s *command;
	struct commlist_s *commands;
	struct toklist_s *list;
}

%destructor {
	for(int i = 1; i < $$->argc; i++) {
		free($$->argv[i]);
	}
	free($$->argv);
	free($$->command);
	free($$);
} <command>

%destructor {
	struct commentry_s *np;
	while (!SLIST_EMPTY(&$$->head)) {
			np = SLIST_FIRST(&$$->head);
			SLIST_REMOVE_HEAD(&$$->head, entries);
			free(np->command);
			free(np);
	}
	free($$);
} <commands>


%token <token> TOKEN "identifier"
%token SEMICOLON ";"
%token OUT ">"
%token IN "<"
%token PIPE "pipe"
%token END_OF_LINE "new line"
%token END_OF_FILE "EOF"

%type <list> args;
%type <command> command;
%type <commands> commands;

%%
input:
	lines {YYACCEPT;};

lines:
	| line lines;

line:
	 command commands end {
		struct commentry_s *np;
		comm_handle($1->command, $1->argc, $1->argv);
		free($1);
		while (!SLIST_EMPTY(&$2->head)) {
				np = SLIST_FIRST(&$2->head);
				SLIST_REMOVE_HEAD(&$2->head, entries);
				comm_handle(np->command->command, np->command->argc, np->command->argv);
				free(np->command);
				free(np);
		}
		free($2);
	};

end:
	END_OF_LINE | END_OF_FILE;

commands:{
		struct commlist_s *list = malloc(sizeof(struct commlist_s));
		struct commlisth_s head = SLIST_HEAD_INITIALIZER(head);
		list->size = 0;
		list->head = head;
		SLIST_INIT(&list->head); 
		$$ = list;
	}
	| SEMICOLON command commands {
		struct commentry_s *tmp = malloc(sizeof(struct commentry_s));
		tmp->command = $2;
		SLIST_INSERT_HEAD(&$3->head, tmp, entries);
		$3->size++;
		$$ = $3;
	}
	| 
	SEMICOLON
	{
		struct commlist_s *list = malloc(sizeof(struct commlist_s));
		struct commlisth_s head = SLIST_HEAD_INITIALIZER(head);
		list->size = 0;
		list->head = head;
		SLIST_INIT(&list->head); 
		$$ = list;
	};

command:
	TOKEN args {
		char **args = malloc(sizeof(char*)*($2->size+2));
		struct toklisth_s head = $2->head;
		args[0] = basename($1);
		int i = 1;
		struct tokentry_s *np;
		while (!SLIST_EMPTY(&head)) {
			np = SLIST_FIRST(&head);
			args[i++] = np->token;
			SLIST_REMOVE_HEAD(&head, entries);
			free(np);
		}
		args[i] = NULL;
		free($2);
		struct command_s *data = malloc(sizeof(struct command_s));
		data->command = $1;
		data->argc = i;
		data->argv = args;
		$$ = data;
	};

args: {
	struct toklist_s *list = malloc(sizeof(struct toklist_s));
	struct toklisth_s head = SLIST_HEAD_INITIALIZER(head);
	list->size = 0;
	list->head = head;
	SLIST_INIT(&list->head); 
	$$ = list;
	}
	| TOKEN args {
	struct tokentry_s *tmp = (struct tokentry_s*) malloc(sizeof(struct tokentry_s));
	tmp->token = $1;
	SLIST_INSERT_HEAD(&$2->head, tmp, entries);
	$2->size++;
	$$ = $2;
};

%%

void yyerror(const char *s) {
	fprintf(stderr, "error:%d: %s\n", line, s);
	EXIT_CODE = 254;
}