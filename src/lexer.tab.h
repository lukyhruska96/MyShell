#ifndef LEXER_TAB_H
#define LEXER_TAB_H

#include <string.h>
#include <stdlib.h>

struct lex_token_s
{
    char* val;
};

struct lex_pipe_s {};

struct lex_semicolon_s {};

struct lex_out_s {};

struct lex_in_s {};

struct lex_token_s
lex_make_token(char* yytext);

struct lex_pipe_s
lex_make_pipe(char* yytext);

struct lex_semicolon_s
lex_make_semicolon(char* yytext);

struct lex_out_s
lex_make_out(char* yytext);

struct lex_in_s
lex_make_in(char* yytext);

#endif //LEXER_TAB_H