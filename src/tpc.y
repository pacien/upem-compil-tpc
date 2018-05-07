%{
/*
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 *
 *	TODO :
 *  ------
 * - Gérer les globales avec .bss (Il faut donc décaler le début du programme après l'analyse des globales pour savoir combien de place réserver.)
 * - Gestion des tableaux
 * - Tableau des fonctions
 *
 */

#include <stdio.h>
#include "symbol_table.h"
#include "generator.h"

extern int lineno;
int yylex();
void yyerror(char *);
static Scope scope = GLOBAL;
static int num_label = 0;
static int num_if = 0;
%}

%union {
  char caractere;
  int num;
  char ident[64];
  int type;
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

%type <num> Exp EB TB FB M E T F
%type <ident> LValue

%left ','
%precedence ')'
%precedence ELSE

%%
Prog:                              { gen_prologue(); }
    DeclConsts DeclVars DeclFoncts { gen_const_declaration(); }
  ;
DeclConsts:
    DeclConsts CONST ListConst ';'
  |
  ;
ListConst:
    ListConst ',' IDENT '=' Litteral
  | IDENT '=' Litteral
  ;
Litteral:
    NombreSigne
  | CARACTERE
  ;
NombreSigne:
    NUM
  | ADDSUB NUM
  ;
DeclVars:
    DeclVars TYPE Declarateurs ';'
  |
  ;
Declarateurs:
    Declarateurs ',' Declarateur { gen_declaration($<ident>3, $<type>0, scope); }
  | Declarateur                  { gen_declaration($<ident>1, $<type>0, scope); }
  ;
Declarateur:
    IDENT
  | IDENT '[' NUM ']'
  ;
DeclFoncts:
     DeclFoncts DeclFonct
  |  DeclFonct
  ;
DeclFonct:
    EnTeteFonct { scope = LOCAL; }
    Corps       { scope = GLOBAL; }
  ;
EnTeteFonct:
    TYPE IDENT '(' Parametres ')'
  | VOID IDENT '(' Parametres ')'
  ;
Parametres:
    VOID
  | ListTypVar
  ;
ListTypVar:
    ListTypVar ',' TYPE IDENT { gen_declaration($<ident>4, $<type>3, scope); }
  | TYPE IDENT                { gen_declaration($<ident>2, $<type>1, scope); }
  ;
Corps:
    '{' DeclConsts DeclVars SuiteInstr '}'
  ;
SuiteInstr:
    SuiteInstr Instr
  |
  ;
Instr:
    Exp ';'
  | ';'
  | RETURN Exp ';'
  | RETURN ';'
  | READE '(' IDENT ')' ';'        { gen_read($<ident>3, scope); }
  | READC '(' IDENT ')' ';'        { gen_read($<ident>3, scope); }
  | PRINT '(' Exp ')' ';'          { gen_print();}
  | IF '(' Exp IfHandling')' Instr { gen_if_label($<num>4); }
  | IF '(' Exp IfHandling')' Instr ELSE IfEndHandling Instr IfElseEndHandling
  | WHILE '(' Exp ')' Instr
  | '{' SuiteInstr '}'
  ;
IfHandling:                       { gen_if_start($<num>$ = num_if++); };
IfEndHandling:                    { gen_if_end($<num>-3); };
IfElseEndHandling:                { gen_ifelse_end($<num>-5); };
Exp:
    LValue '=' Exp               { /* TODO: extract this into func */
                                    if(scope == GLOBAL){
                                    $$ = glo_lookup($<ident>1);
                                    printf("pop QWORD [rbp - %d] ;%s\n",glo_get_addr($<ident>1),$<ident>1);
                                    }
                                    else {
                                      $$ = loc_lookup($<ident>1);
                                      printf("pop QWORD [rbp - %d] ;%s\n",loc_get_addr($<ident>1),$<ident>1);
                                    }
                                  }
  | EB
  ;
EB:
    EB OR TB    { gen_or($1, $3, num_label++); }
  | TB
  ;
TB:
    TB AND FB   { gen_and($1, $3, num_label++); }
  | FB
  ;
FB:
    FB EQ M     { gen_eq($2, $1, $3, num_label++); }
  | M
  ;
M:
    M ORDER E   { gen_order($2, $1, $3, num_label++); }
  |  E
  ;
E:
    E ADDSUB T  { gen_addsub($2, $1, $3); }
  | T
  ;
T:
    T DIVSTAR F { gen_divstar($2, $1, $3); }
  | F
  ;
F:
     ADDSUB F                     {$$ = $2;//on fait remonter le type
                                  if($1 == '+') {
                                    printf(";+F\n");
                                  }
                                  else{
                                    printf(";-F\npop rdx\nxor rax,rax\nsub rax,rdx\npush rax\n");
                                  }
                                  }
  |  '!' F                        {$$ = $2;printf(";!F\npop rax\nxor rax,1\npush rax\n");}
  |  '(' Exp ')'                  {$$ = $2;}
  |  LValue                       {
                                    if(scope == GLOBAL) {
                                      $$ = glo_lookup($<ident>1);
                                      printf("push QWORD [rbp - %d] ;%s\n",glo_get_addr($<ident>1),$<ident>1);
                                    }
                                    else {
                                      $$ = loc_lookup($<ident>1);
                                      printf("push QWORD [rbp - %d] ;%s\n",loc_get_addr($<ident>1),$<ident>1);
                                    }
                                  }
  |  NUM                          {$$ = INT; if(scope == LOCAL) printf("push %d\n",$1);} // on stocke les types pour l'analyse sémantique
  |  CARACTERE                    {$$ = CHAR; if(scope == LOCAL) printf("push %d\n",$1);}
  |  IDENT '(' Arguments  ')'     {$$ = INT;} //tableau d'entiers uniquement
  ;
LValue:
     IDENT                        {
                                    if(scope == GLOBAL) {
                                      glo_lookup($<ident>1);
                                    }
                                    else {
                                      loc_lookup($<ident>1);
                                    }
                                  }
  |  IDENT  '[' Exp  ']'          {
                                    if(scope == GLOBAL) {
                                      glo_lookup($<ident>1);
                                    }
                                    else {
                                      loc_lookup($<ident>1);
                                    }
                                  }
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
