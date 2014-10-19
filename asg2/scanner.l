%{
// $Id: scanner.l,v 1.3 2013-10-15 16:37:56-07 - - $

#include "auxlib.h"
#include "lyutils.h"

#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }


%}

%option 8bit
%option debug
%option ecs
%option nodefault
%option nounput
%option noyywrap
%option perf-report
%option verbose
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]
SYM \[\]|\(|\)|\[|\]|\{|\}|\;|\,|\.|=|==|!=|<|<=|>|>=|+|-|\*|/|%|!
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)

PREPROC #{NUMBER} {STRING}
RESERVED void|bool|char|int|string|struct|if|else|while|return|new|false|true|null|ord|chr
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
INVALID_IDENT {NUMBER}+{IDENT}
CHAR (’([^\\’\n]|\\[\\’"0nt])’)
STRING (\"([^\\"\n]|\\[\\’"0nt])*\")\
INVALID_CHAR '['\\n]|{CHAR}{2}
INVALID_STRING  "{INVALID_CHAR}*"|".*


%%

"#".*           { scanner_include(); }
[ \t]+          { IGNORE (white space) }
\n              { scanner_newline(); }

{NUMBER}        { return yylval_token (NUMBER); }
{IDENT}         { return yylval_token (IDENT); }

"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"^"             { return yylval_token ('^'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }

{NOTNUMBER}     { scanner_badtoken (yytext);
                  return yylval_token (NUMBER); }

.               { scanner_badchar (*yytext); }

%%

RCSC("$Id: scanner.l,v 1.3 2013-10-15 16:37:56-07 - - $")

