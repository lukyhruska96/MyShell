%{
    #include <stdio.h>
    #include <sys/queue.h>
    #include <stdlib.h>
    #include <string.h>
    #include "command.h"

    // Declare stuff from Flex that Bison needs to know about:
    extern int yylex();
    void yyerror(const char *s);

    SLIST_HEAD(toklisth_s, tokentry_s);

    struct toklist_s {
        int size;
        struct toklisth_s head;
    };

    struct tokentry_s {
        SLIST_ENTRY(tokentry_s) entries;
        char *token;
    };
%}

%union {
    char *token;
    struct toklist_s *list;
}

%token <token> TOKEN
%token SEMICOLON
%token OUT
%token IN
%token PIPE
%token END_OF_LINE
%token END_OF_FILE

%type <list> args;

%%
input:
    line lines {YYACCEPT;};

lines:
    | END_OF_LINE line lines;

line:
    | command commands end;

end:
    | END_OF_FILE;

commands:
    | SEMICOLON command commands;

command: 
    TOKEN args {
        char **args = malloc(sizeof(char*)*$2->size);
        struct toklisth_s head = $2->head;
        int i = 0;
        struct tokentry_s *np;
        while (!SLIST_EMPTY(&head)) {
             np = SLIST_FIRST(&head);
             args[i++] = np->token;
             SLIST_REMOVE_HEAD(&head, entries);
             free(np);
        }
        free($2);
        comm_handle($1, i, args);
    };

args: {
    struct toklist_s *list = malloc(sizeof(struct toklist_s));
    struct toklisth_s head = SLIST_HEAD_INITIALIZER(head);
    list->size = 0;
    list->head = head;
    SLIST_INIT(&list->head); 
    $$ = list;
    }| TOKEN args {
    struct tokentry_s *tmp = (struct tokentry_s*) malloc(sizeof(struct tokentry_s));
    tmp->token = $1;
    SLIST_INSERT_HEAD(&$2->head, tmp, entries);
    $2->size++;
    $$ = $2;
};

%%

void yyerror(const char *s) {
  printf("Parse error!  Message: %s\n", s);
  exit(-1);
}