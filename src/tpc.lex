%{
/**
  * UPEM / Compilation / Projet
  * Pacien TRAN-GIRARD, Adam NAILI
  */

#include "tpc.tab.h"

int lineno = 1;
%}

%option noinput nounput noyywrap
%x COMMENT

%%
[ \t\r]+                ;
\n                      { lineno++; }
"/*"                    { BEGIN COMMENT; }
&&                      { return AND; }
"||"                    { return OR; }
"*"|"/"|%               { return DIVSTAR; }
"+"|-                   { return ADDSUB; }
"<"|"<="|">"|>=         { return ORDER; }
==|!=                   { return EQ; }
int                     { return TYPE; }
float                   { return TYPE; }
double                  { return TYPE; }
char                    { return TYPE; }
void                    { return VOID; }
const                   { return CONST; }
if                      { return IF; }
else                    { return ELSE; }
while                   { return WHILE; }
return                  { return RETURN; }
[a-zA-Z_][a-zA-Z0-9_]*  { return IDENT; }
[0-9]+                  { return NUM; }
'\\?.'                  { return CARACTERE; }
.                       { return yytext[0]; }
<COMMENT>"*/"           { BEGIN INITIAL; }
<COMMENT>\n             { lineno++; }
<COMMENT>.              ;
%%

