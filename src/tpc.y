%{
/*
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#include <stdio.h>
#include "symboltable.h"

extern int lineno;
int yylex();
void yyerror(char *);

%}

%union {
    char caractere;
    int num;
    char ident[64];
    char type[32];
    char comp[3];
    char addsub;
    char divstar;
}
%token <caractere> CARACTERE
%token <num> NUM
%token <ident> IDENT
%token <comp> ORDER EQ
%token <addsub> ADDSUB
%token <divstar> DIVSTAR
%token OR AND CONST IF ELSE WHILE RETURN VOID PRINT READC READE
%token <type> TYPE

%type <num> Exp EB TB FB M E T F LValue

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
     ADDSUB F                   {$$ = $2;} //on fait remonter le type
  |  '!' F                      {$$ = $2;} 
  |  '(' Exp ')'                {$$ = $2;}
  |  LValue                     //Il faut trouver le type de LValue
  |  NUM                        {$$ = INT;} // on stocke les types pour l'analyse sémantique
  |  CARACTERE                  {$$ = CHAR;}
  |  IDENT '(' Arguments  ')'   {$$ = INT;} //tableau d'entiers uniquement
  ;
LValue:
     IDENT                      {lookup($<ident>1);}
  |  IDENT  '[' Exp  ']'        {lookup($<ident>1);}
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

