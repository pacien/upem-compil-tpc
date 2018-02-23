%{
/*
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#include <stdio.h>

extern int lineno;
int yylex();
void yyerror(char *);

%}

%token CARACTERE NUM IDENT
%token ADDSUB DIVSTAR
%token ORDER EQ OR AND
%token IF WHILE RETURN
%token CONST VOID TYPE
%token PRINT READC READE

%left ','
%precedence ')'
%precedence ELSE

%%
Prog:
     DeclConsts DeclVars DeclFoncts
  ;
DeclConsts:
     DeclConsts CONST ListConst ';'
  |
  ;
ListConst:
     ListConst ',' IDENT '=' Litteral
  |  IDENT '=' Litteral
  ;
Litteral:
     NombreSigne
  |  CARACTERE
  ;
NombreSigne:
     NUM
  |  ADDSUB NUM
  ;
DeclVars:
     DeclVars TYPE Declarateurs ';'
  |
  ;
Declarateurs:
     Declarateurs ',' Declarateur
  |  Declarateur
  ;
Declarateur:
     IDENT
  |  IDENT '[' NUM ']'
  ;
DeclFoncts:
     DeclFoncts DeclFonct
  |  DeclFonct
  ;
DeclFonct:
     EnTeteFonct Corps
  ;
EnTeteFonct:
     TYPE IDENT '(' Parametres ')'
  |  VOID IDENT '(' Parametres ')'
  ;
Parametres:
     VOID
  |  ListTypVar
  ;
ListTypVar:
     ListTypVar ',' TYPE IDENT
  |  TYPE IDENT
  ;
Corps:
     '{' DeclConsts DeclVars SuiteInstr '}'
  ;
SuiteInstr:
     SuiteInstr Instr
  |  ;
Instr:
     Exp ';'
  |  ';'
  |  RETURN Exp ';'
  |  RETURN ';'
  |  READE '(' IDENT ')' ';'
  |  READC '(' IDENT ')' ';'
  |  PRINT '(' Exp ')' ';'
  |  IF '(' Exp ')' Instr
  |  IF '(' Exp ')' Instr ELSE Instr
  |  WHILE '(' Exp ')' Instr
  |  '{' SuiteInstr '}'
  ;
Exp:
     LValue '=' Exp
  |  EB
  ;
EB:
     EB OR TB
  |  TB
  ;
TB:
     TB AND FB
  |  FB
  ;
FB:
     FB EQ M
  |  M
  ;
M:
     M ORDER E
  |  E
  ;
E:
     E ADDSUB T
  |  T
  ;
T:
     T DIVSTAR F
  |  F
  ;
F:
     ADDSUB F
  |  '!' F
  |  '(' Exp ')'
  |  LValue
  |  NUM
  |  CARACTERE
  |  IDENT '(' Arguments  ')'
  ;
LValue:
     IDENT
  |  IDENT  '[' Exp  ']'
  ;
Arguments:
     ListExp
  |
  ;
ListExp:
     ListExp ',' Exp
  |  Exp
  ;
%%

void yyerror(char *msg){
  fprintf(stderr, "%s at line %d\n", msg, lineno);
}

int main(int argc, char **argv) {
  return yyparse();
}

