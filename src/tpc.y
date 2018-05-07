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
Prog: { prologue(); }
    DeclConsts DeclVars DeclFoncts { const_declaration(); }
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
    Declarateurs ',' Declarateur { declaration($<ident>3, $<type>0, scope); }
  | Declarateur                  { declaration($<ident>1, $<type>0, scope); }
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
    ListTypVar ',' TYPE IDENT { declaration($<ident>4, $<type>3, scope); }
  | TYPE IDENT                { declaration($<ident>2, $<type>1, scope); }
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
  |  READE '(' IDENT ')' ';'      {
                                    if(scope == GLOBAL) glo_lookup($<ident>3);
                                    else loc_lookup($<ident>3);
                                  }
  |  READC '(' IDENT ')' ';'      {
                                    if(scope == GLOBAL) glo_lookup($<ident>3);
                                    else loc_lookup($<ident>3);
                                  }
  |  PRINT '(' Exp ')' ';'        {printf("pop rax\ncall print\n");}
  |  IF '(' Exp IfHandling')' Instr {printf(".end_if%d:\n;ENDIF\n\n",$<num>4);}
  |  IF '(' Exp IfHandling')' Instr ELSE IfEndHandling Instr IfElseEndHandling
  |  WHILE '(' Exp ')' Instr
  |  '{' SuiteInstr '}'
  ;
IfHandling:                       {printf("\n;BEGINIF\npop rax\ncmp rax,0\njz .end_if%d\n",$<num>$ = num_if++);};
IfEndHandling:                    {printf("jmp .end_ifelse%d\n.end_if%d:\n",$<num>-3,$<num>-3);};
IfElseEndHandling:                {printf(".end_ifelse%d:\n;ENDIF\n\n",$<num>-5);};
Exp:
     LValue '=' Exp               {
                                    if(scope == GLOBAL){
                                    $$ = glo_lookup($<ident>1);
                                    printf("pop QWORD [rbp - %d] ;%s\n",glo_get_addr($<ident>1),$<ident>1);
                                    }
                                    else {
                                      $$ = loc_lookup($<ident>1);
                                      printf("pop QWORD [rbp - %d] ;%s\n",loc_get_addr($<ident>1),$<ident>1);
                                    }
                                  }
  |  EB
  ;
EB:
     EB OR TB                     {
                                    check_expected_type($1,INT);check_expected_type($3,INT);
                                    printf(";a OR c\npop rax\n");
                                    printf("cmp rax,1\nje .true%d\npop rcx\ncmp rcx,1\njz .true%d\npush 0\njmp .false%d\n",num_label,num_label,num_label);
                                    printf(".true%d:\npush 1\n.false%d:",num_label,num_label);
                                    num_label++;
                                  }
  |  TB
  ;
TB:
     TB AND FB                    {
                                    check_expected_type($1,INT);check_expected_type($3,INT);
                                    printf(";a AND c\npop rax\n");
                                    printf("cmp rax,0\njz .false%d\npop rcx\ncmp rcx,0\njz .false%d\npush 1\njmp .true%d\n",num_label,num_label,num_label);
                                    printf(".false%d:\npush 0\n.true%d:",num_label,num_label);
                                    num_label++;
                                  }
  |  FB
  ;
FB:
     FB EQ M                      {
                                    check_expected_type($1,INT);check_expected_type($3,INT);
                                    printf(";a EQ c\npop rax\npop rcx\ncmp rax,rcx\n");
                                    if(!strcmp($2,"==")){
                                      printf("je .true%d\n",num_label);
                                    }else{
                                      printf("jne .true%d\n",num_label);
                                    }
                                    printf("push 0\njmp .false%d\n.true%d:\npush 1\n.false%d:",num_label,num_label,num_label);
                                    num_label++;
                                  }
  |  M
  ;
M:
     M ORDER E                    {
                                    check_expected_type($1,INT);check_expected_type($3,INT);
                                    printf(";a ORDER c\npop rcx\npop rax\ncmp rax,rcx\n");
                                    if(!strcmp($2,"<")){
                                      printf("jl .true%d\n",num_label);
                                    }else if(!strcmp($2,"<=")){
                                      printf("jle .true%d\n",num_label);
                                    }else if(!strcmp($2,">")){
                                      printf("jg .true%d\n",num_label);
                                    }else{
                                      printf("jge .true%d\n",num_label);
                                    }
                                    printf("push 0\njmp .false%d\n.true%d:\npush 1\n.false%d:",num_label,num_label,num_label);
                                    num_label++;
                                  }
  |  E
  ;
E:
     E ADDSUB T                   {
                                    check_expected_type($1,INT);check_expected_type($3,INT);
                                    if($2 == '+'){
                                      printf(";E + T\npop rcx\npop rax\nadd rax,rcx\npush rax\n");
                                    }
                                    else{
                                      printf(";E - T\npop rcx\npop rax\nsub rax,rcx\npush rax\n");
                                    }
                                  }
  |  T
  ;
T:
     T DIVSTAR F                  {
                                    check_expected_type($1,INT);check_expected_type($3,INT);
                                    if($2 == '*'){
                                      printf(";E * T\npop rax\npop rcx\nimul rax,rcx\npush rax\n");
                                    }
                                    else{
                                      printf(";E / T\npop rax\npop rcx\nxor rdx,rdx\nidiv rcx\n");
                                      if ($2 == '/'){
                                      	printf("push rax\n");
                                      }
                                      else{
                                      	printf("push rdx\n");
                                      }
                                    }
                                  }
  |  F
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
