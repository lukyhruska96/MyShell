%{
	#include "parser.tab.h"
	#include <stdio.h>
	#include "shell.h"

	int line = 1;
%}
%option noyywrap
%option nounput
%option noinput

WHITESPACE		[ \t]
APPEND			\>\>
OUT				\>
IN				\<
token			[^ \t\n\>\<\#\;]+

%x comment

%%
<comment>[^\n]+

{WHITESPACE}+
{OUT}			{ return (OUT); }
{IN}			{ return (IN); }
{APPEND}		{ return (APPEND); }
\|				{ return (PIPE); }
\;				{ return (SEMICOLON); }
\#              { BEGIN(comment); }
{token}			{ yylval.token = strdup(yytext); return (TOKEN); }
<INITIAL,comment>\n	{ line++; BEGIN(0); return (END_OF_LINE); }

<<EOF>>			{ return (END_OF_FILE); }


%%