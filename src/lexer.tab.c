#include "lexer.tab.h"

struct lex_token_s
lex_make_token(char* txt)
{
    size_t len = strlen(txt);
    char* tmp = (char*) malloc(len);
    strcpy(tmp, txt);
    struct lex_token_s token = { tmp };
    return (token);
}

struct lex_pipe_s
lex_make_pipe(char* yytext)
{
    struct lex_pipe_s pipe = {};
    return (pipe);
}

struct lex_semicolon_s
lex_make_semicolon(char* yytext)
{
    struct lex_semicolon_s semicolon = {};
    return (semicolon);
}

struct lex_out_s
lex_make_out(char* yytext)
{
    struct lex_out_s out = {};
    return (out);
}

struct lex_in_s
lex_make_in(char* yytext)
{
    struct lex_in_s in = {};
    return (in);
}