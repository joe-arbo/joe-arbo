/*
 * Tokens for the shell.
 *
 * Updated Summer 2020.
 * Developed by Godmar Back for CS 3214 Fall 2009
 * Virginia Tech.
 */
%{
#include <string.h>
%}
%%
[ \t]*		;
">>"		return GREATER_GREATER;
">&"		return GREATER_AMPERSAND;
"|&"		return PIPE_AMPERSAND;
[|&;<>\n]	return *yytext;
\"([^\\\"]|\\.)*\"  {   // a quoted token using double quotes
    char * word = strdup(yytext+1); // skip leading "
    word[strlen(word)-1] = '\0';    // trim trailing "
    yylval.word = word;
    return WORD; 
}
[^|&;<>\n\t ]+ 	{ yylval.word = strdup(yytext); return WORD; }
%%
