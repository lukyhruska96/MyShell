%{
    #include "lexer.tab.h"
%}

whitespace      [\s]
newline         \n
out             \>
in              \<
token           [^{whitespace}{in}{out}{newline}]+

%%
{newline}       
{out}           { return lex_make_out(yytext); }
{in}            { return lex_make_in(yytext); }
\|              { return lex_make_pipe(yytext); }
{token}         { return lex_make_token(yytext); }
{whitespace}+   

%%