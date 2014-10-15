%{
#include <stdlib.h>
#include <stdio.h>
}%

%option 8bit
%option debug
%option ecs
%option nodefault
%option nounput
%option noyywrap
%option perf-report
%option verbose
%option warn

DIGIT [0-9]
ALPHA ([A-Z]|[a-z])
SYM \[\]|\(|\)|\[|\]|\{|\}|\;|\,|\.|=|==|!=|<|<=|>|>=|+|-|\*|/|%|!
RESERVED void|bool|char|int|string|struct|if|else|while|return|new|false|true\
|null|ord|chr
IDENT {ALPHA}+({ALPHA}|{DIGIT})*
INT {DIGIT}+
CHAR '(.|^[\,\n,'])'
STRING "({CHAR})*"



%%

