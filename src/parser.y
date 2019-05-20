%{
	#include <errno.h>
	#include <stdio.h>
	#include <sys/queue.h>
	#include <stdlib.h>
	#include <string.h>
	#include <libgen.h>
	#include <unistd.h>
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
		char ** redirections;
		int pd[2];
	};
%}

%error-verbose

%union {
	char *token;
	char **redirections;
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
%token APPEND ">>"
%token IN "<"
%token PIPE "pipe"
%token END_OF_LINE "new line"
%token END_OF_FILE "EOF"

%type <list> args;
%type <redirections> redirections;
%type <command> command;
%type <commands> commands;

%%
input:
	lines {YYACCEPT;};

lines:
	| line lines;

line:
	SEMICOLON end
	| command commands end {
		struct commentry_s *np = malloc(sizeof(struct commentry_s));
		if(np == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
		struct commentry_s *np_next;
		np->command = $1;
		int pd[2] = {0, 0};
		while (!SLIST_EMPTY(&$2->head)) {
				np_next = SLIST_FIRST(&$2->head);
				SLIST_REMOVE_HEAD(&$2->head, entries);
				pd[1] = np_next->command->pd[1];
				comm_handle(np->command->command, np->command->argc, np->command->argv, (int*) &pd, np->command->redirections);
				if(pd[0]) close(pd[0]);
				if(pd[1]) close(pd[1]);
				pd[0] = np_next->command->pd[0];
				free(np->command);
				free(np);
				np = np_next;
		}
		pd[1] = 0;
		comm_handle(np->command->command, np->command->argc, np->command->argv, (int*) &pd, np->command->redirections);
		if(pd[0]) close(pd[0]);
		free(np->command);
		free(np);
		free($2);
	};

end:
	END_OF_LINE | END_OF_FILE;

commands: {
		struct commlist_s *list = malloc(sizeof(struct commlist_s));
		if(list == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
		struct commlisth_s head = SLIST_HEAD_INITIALIZER(head);
		list->size = 0;
		list->head = head;
		SLIST_INIT(&list->head); 
		$$ = list;
	}
	| PIPE command commands {
		struct commentry_s *tmp = malloc(sizeof(struct commentry_s));
		if(tmp == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
		tmp->command = $2;
		pipe(tmp->command->pd);
		SLIST_INSERT_HEAD(&$3->head, tmp, entries);
		$3->size++;
		$$ = $3;
	}
	| SEMICOLON command commands {
		struct commentry_s *tmp = malloc(sizeof(struct commentry_s));
		if(tmp == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
		tmp->command = $2;
		SLIST_INSERT_HEAD(&$3->head, tmp, entries);
		$3->size++;
		$$ = $3;
	}
	| 
	SEMICOLON
	{
		struct commlist_s *list = malloc(sizeof(struct commlist_s));
		if(list == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
		struct commlisth_s head = SLIST_HEAD_INITIALIZER(head);
		list->size = 0;
		list->head = head;
		SLIST_INIT(&list->head); 
		$$ = list;
	};

command:
	TOKEN args redirections {
		char **args = malloc(sizeof(char*)*($2->size+2));
		if(args == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
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
		if(data == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
		data->command = $1;
		data->argc = i;
		data->argv = args;
		data->pd[0] = 0;
		data->pd[1] = 0;
		data->redirections = $3;
		$$ = data;
	};

args: {
	struct toklist_s *list = malloc(sizeof(struct toklist_s));
	if(list == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
	struct toklisth_s head = SLIST_HEAD_INITIALIZER(head);
	list->size = 0;
	list->head = head;
	SLIST_INIT(&list->head); 
	$$ = list;
	}
	| TOKEN args {
	struct tokentry_s *tmp = (struct tokentry_s*) malloc(sizeof(struct tokentry_s));
	if(tmp == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
	tmp->token = $1;
	SLIST_INSERT_HEAD(&$2->head, tmp, entries);
	$2->size++;
	$$ = $2;
};

redirections: {
		$$ = (char**) malloc(sizeof(char*)*3);
		if($$ == NULL && errno == ENOMEM)
			fprintf(stderr, "Out  of  memory.\n");
		$$[0] = NULL;
		$$[1] = NULL;
		$$[2] = NULL;
	}
	| APPEND TOKEN redirections { if($3[2] == NULL && $3[1] == NULL) $3[2] = $2; $$ = $3; }
	| IN TOKEN redirections { if($3[0] == NULL) $3[0] = $2; $$ = $3; }
	| OUT TOKEN redirections { if($3[2] == NULL && $3[1] == NULL) $3[1] = $2; $$ = $3; }

%%

void yyerror(const char *s) {
	fprintf(stderr, "error:%d: %s\n", line, s);
	EXIT_CODE = 254;
}