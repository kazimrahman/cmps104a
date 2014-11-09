%{
// Dummy parser for scanner project.

#include "lyutils.h"
#include "astree.h"
#include <assert.h>
//fix variable
%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT

%right TOK_IFELSE 
%right '=' 
%left TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left '+' '-'
%left '*' '/' '%'
%right TOK_ORD TOK_CHR

%start root

%%
root     : program      {yyparse_astree = $1;}
         ;

program  : program structdef     {}
         | program function      {}
         | program statement     {}
         | program error '}'     {}
         | program error ';'     {}
         |                       {$$ = new_parseroot();}
         ;


structdef: structroot '}'     {}
         ;

structroot  : structroot TOK_FIELD            {}
            | TOK_STRUCT TOK_TYPEID '{'        {}
            ;

fielddelc: basetype TOK_ARRAY TOK_FIELD  {}
         | basetype TOK_FIELD            {}
         ;

basetype : TOK_VOID        {}
         | TOK_BOOL        {}
         | TOK_CHAR        {}
         | TOK_INT         {}
         | TOK_STRING      {}
         | TOK_TYPEID      {}
         ;   

function : funcroot ')' block       {}
         ;

funcroot : identdecl '('            {}
         | funcroot ',' TOK_IDENT   {}
         ;

identdecl: basetype TOK_ARRAY TOK_IDENT {}
         | basetype TOK_IDENT           {}

block    : '{' statement         {}
         | block ',' statement   {}
         | block '}'             {}
         | ';'                   {}
         ;

stateroot: stateroot statement {}
         | statement           {}
         ;

statement: block        {}
         | vardecl      {}
         | while        {}
         | ifelse       {}
         | return       {}
         | expr ';'     {}
         ;
      
vardecl  : identdecl '=' expr ';'                           {}
         ;

while    : TOK_WHILE '(' expr ')' statement     {$$ = adopt2($2, $1, $3);}
         ;

ifelse   : TOK_IF '(' expr ')' statement                    {$$ = adopt2($1, $2, $3);}
         | TOK_IF '(' expr ')' statement TOK_ELSE statement {}
         ;

return   : TOK_RETURN expr ';'   {$$ = adopt1($1, $2);}
         | TOK_RETURN            {}
         ;

expr     : allocator          {$$ = $1}
         | constant           {$$ = $1}
         | variable           {$$ = $1}
         | '(' expr ')'       {$$ = $1}
         | expr TOK_IFELSE expr  {$$ = adopt2($2, $1, $3);}    
         | expr TOK_EQ expr      {$$ = adopt2($2, $1, $3);}      
         | expr TOK_NE expr      {$$ = adopt2($2, $1, $3);}    
         | expr TOK_LT expr      {$$ = adopt2($2, $1, $3);}
         | expr TOK_LE expr      {$$ = adopt2($2, $1, $3);}
         | expr TOK_GT expr      {$$ = adopt2($2, $1, $3);}
         | expr TOK_GE expr      {$$ = adopt2($2, $1, $3);}
         | expr '+' expr         {$$ = adopt2($2, $1, $3);}
         | expr '-' expr         {$$ = adopt2($2, $1, $3);}
         | expr '*' expr         {$$ = adopt2($2, $1, $3);}
         | expr '/' expr         {$$ = adopt2($2, $1, $3);}
         | expr '%' expr         {$$ = adopt2($2, $1, $3);}
         ;


allocator: TOK_NEW TOK_TYPEID    {$$ = adopt1($1, $2);}
         | TOK_NEW TOK_STRING '(' expr ')'            {}
         | TOK_NEW basetype '[' expr ']'              {}
         ;

call     : TOK_IDENT callroot ')'  {}
         | TOK_IDENT '(' ')'        {}
         ;

callroot : '(' expr          {}
         | callroot ',' expr {}
         | '('               {}
         ;

variable : TOK_IDENT             {$$ = $1;} 
         | expr '[' expr ']'     {free_ast2($2, $4); $$ = adopt1($1, $3);}
         | expr'.' TOK_IDENT     {$$ = adopt1($1, $3);}
         ;   

constant : TOK_INTCON      {$$ = $1;}
         | TOK_CHARCON     {$$ = $1;}
         | TOK_STRINGCON   {$$ = $1;}
         | TOK_FALSE       {$$ = $1;}
         | TOK_TRUE        {$$ = $1;}
         | TOK_NULL        {$$ = $1;}
         ;
%%

const char *get_yytname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}


bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}

