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
"&&"                    { return AND; }
"||"                    { return OR; }
"*"|"/"|%               { yylval.divstar=yytext[0]; return DIVSTAR; }
"+"|-                   { yylval.addsub=yytext[0]; return ADDSUB; }
"<"|"<="|">"|">="       { strcpy(yylval.comp, yytext); return ORDER; }
==|!=                   { strcpy(yylval.comp, yytext); return EQ; }
entier                  { strcpy(yylval.type, yytext); return TYPE; }
caractere               { strcpy(yylval.type, yytext); return TYPE; }
void                    { return VOID; }
const                   { return CONST; }
if                      { return IF; }
else                    { return ELSE; }
while                   { return WHILE; }
return                  { return RETURN; }
print                   { return PRINT; }
readc                   { return READC; }
reade                   { return READE; }
[a-zA-Z_][a-zA-Z0-9_]*  { strcpy(yylval.ident, yytext); return IDENT; }
[0-9]+                  { sscanf(yytext, "%d", &(yylval.num)); return NUM; }
'\\?.'                  { if (strlen(yytext)==3)
                            yylval.caractere=yytext[1];
                          else switch(yytext[2]) {
                            case 'n': yylval.caractere='\n'; break;
                            case 't': yylval.caractere='\t'; break;
                            case '\'': yylval.caractere='\''; break;
                            }
                          return CARACTERE; }
.                       { return yytext[0]; }
<COMMENT>"*/"           { BEGIN INITIAL; }
<COMMENT>\n             { lineno++; }
<COMMENT>.              ;
%%

