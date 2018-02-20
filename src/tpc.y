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

%union {
  char caractere;
  int num;
  char ident[64];
  char type[16];
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
%token OR AND CONST IF WHILE RETURN VOID
%token <type> TYPE

%left ','
%precedence ')'
%precedence ELSE

%%
Prog         : DeclConsts DeclVars DeclFoncts
             ;
DeclConsts   : DeclConsts CONST ListConst ';'
             |
             ;
ListConst    : ListConst ',' IDENT '=' Litteral
             | IDENT '=' Litteral
             ;
Litteral     : NombreSigne
             | CARACTERE
             ;
NombreSigne  : NUM
             | ADDSUB NUM
             ;
DeclVars     : DeclVars TYPE Declarateurs ';'
             |
             ;
Declarateurs : Declarateurs ',' Declarateur
             | Declarateur
             ;
Declarateur  : IDENT
             | IDENT '[' NUM ']'
             ;
DeclFoncts   : DeclFoncts DeclFonct
             | DeclFonct
             ;
DeclFonct    : EnTeteFonct Corps
             ;
EnTeteFonct  : TYPE IDENT '(' Parametres ')'
             | VOID IDENT '(' Parametres ')'
             ;
Parametres   : VOID
             | ListTypVar
             ;
ListTypVar   : ListTypVar ',' TYPE IDENT
             | TYPE IDENT
             ;
Corps        : '{' DeclConsts DeclVars SuiteInstr '}'
             ;
SuiteInstr   : SuiteInstr Instr
             |
             ;
Instr        : Exp ';'
             | RETURN Exp ';'
             | RETURN ';'
             | ';'
             | IF '(' Exp ')' Instr
             | IF '(' Exp ')' Instr ELSE Instr
             | WHILE '(' Exp ')' Instr
             | '{' SuiteInstr '}'
             ;
Exp          : LValue '=' Exp
             | EB
             ;
EB           : EB OR TB
             | TB
             ;
TB           : TB AND FB
             | FB
             ;
FB           : FB EQ M
             | M
             ;
M            : M ORDER E
             | E
             ;
E            : E ADDSUB T
             | T
             ;
T            : T DIVSTAR F
             | F
             ;
F            : ADDSUB F
             | '!' F
             | '(' Exp ')'
             | LValue
             | NUM
             | CARACTERE
             | IDENT '(' Arguments  ')'
             ;
LValue       : IDENT
             | IDENT  '[' Exp  ']'
             ;
Arguments    : ListExp
             |
             ;
ListExp      : ListExp ',' ListExp
             | Exp
             ;
%%

void yyerror(char *s){
  fprintf(stderr, "Line no%d: %s\n", lineno, s);
}

int main(int argc, char **argv) {
  yyparse();
  return 0;
}

