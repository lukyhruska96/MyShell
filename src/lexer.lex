%{
    #include "parser.tab.h"
    #include <stdio.h>
    #include "shell.h"
%}
%option noyywrap

WHITESPACE      [ \t]
OUT             \>
IN              \<
token           [^ \t\n\>\<\#;]+

%x comment

%%
<comment>[^\n]+

{WHITESPACE}+
{OUT}           { return (OUT); }
{IN}            { return (IN); }
\|              { return (PIPE); }
;               { return (SEMICOLON); }
\#              { BEGIN(comment); }
{token}         { yylval.token = strdup(yytext); return (TOKEN); }
<INITIAL,comment>\n              { return (END_OF_LINE); }

<<EOF>>         { return (END_OF_FILE); }


%%