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
#define GLOBAL 0
#define LOCAL 1
static int status = GLOBAL;

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
Prog:{printf("extern printf\n");
			printf("section .data\n");
			printf("format_int db \"%%d\",10,0\n\n");
			printf("section .bss\nsection .text\n\nglobal _start\n");
			printf("print: ;print needs an argument in rax\n");
			printf("push rbp\n");
			printf("mov rbp, rsp\n");
			printf("push rsi\n");
			printf("mov rsi, rax\n");
			printf("mov rdi, format_int\n");
			printf("mov rax, 0\n");
		  printf("call printf WRT ..plt\n");	
			printf("pop rsi\n");
			printf("pop rbp\n");
			printf("ret\n");
			printf("\n_start:\n");	
      printf("push rbp\nmov rbp, rsp\n\n");
			}
     DeclConsts DeclVars DeclFoncts 
     {
        printf("mov rax,60 \n");
        printf("mov rdi,0 \n");  
        printf("syscall \n");
        printf(";global table\n");
        glo_display_table();
        printf(";local table\n");
        loc_display_table();
     }
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
     Declarateurs ',' Declarateur   {if(status == GLOBAL) glo_addVar($<ident>3, $<type>0);
                                     else loc_addVar($<ident>3, $<type>0);
                                     printf("push 0\n");}
  |  Declarateur                    {if(status == GLOBAL) glo_addVar($<ident>1, $<type>0);
                                     else loc_addVar($<ident>1, $<type>0);
                                     printf("push 0\n");}
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
     EnTeteFonct {status = LOCAL;} Corps {status = GLOBAL;}
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
     ListTypVar ',' TYPE IDENT    {if(status == GLOBAL) glo_addVar($<ident>4, $<type>3);
                                     else loc_addVar($<ident>4, $<type>3);
                                   printf("push 0\n");}
  |  TYPE IDENT                   {if(status == GLOBAL) glo_addVar($<ident>2, $<type>1);
                                     else loc_addVar($<ident>2, $<type>1);
                                   printf("push 0\n");}
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
  |  READE '(' IDENT ')' ';'      {if(status == GLOBAL) glo_lookup($<ident>3);
                                  else loc_lookup($<ident>3);}
  |  READC '(' IDENT ')' ';'      {if(status == GLOBAL) glo_lookup($<ident>3);
                                  else loc_lookup($<ident>3);}
  |  PRINT '(' Exp ')' ';'        {printf("pop rax\ncall print\n");}
  |  IF '(' Exp ')' Instr
  |  IF '(' Exp ')' Instr ELSE Instr
  |  WHILE '(' Exp ')' Instr
  |  '{' SuiteInstr '}'
  ;
Exp:
     LValue '=' Exp               {if(status == GLOBAL){
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
     EB OR TB                     {check_expected_type($1,INT);check_expected_type($3,INT);} 
  |  TB
  ;
TB:
     TB AND FB                    {check_expected_type($1,INT);check_expected_type($3,INT);}
  |  FB
  ;
FB:
     FB EQ M                      {check_expected_type($1,INT);check_expected_type($3,INT);}
  |  M
  ;
M:
     M ORDER E                    {check_expected_type($1,INT);check_expected_type($3,INT);}
  |  E
  ;
E:
     E ADDSUB T                   {check_expected_type($1,INT);check_expected_type($3,INT);
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
     T DIVSTAR F                  {check_expected_type($1,INT);check_expected_type($3,INT);}
  |  F
  ;
F:
     ADDSUB F                     {$$ = $2;//on fait remonter le type
                                  if($1 == '+') {
                                    printf(";+F\n");
                                  }
                                  else{
                                    printf(";-F\npop rdx\nxor eax,eax\nsub eax,edx\npush rax\n");
                                  }
                                  } 
  |  '!' F                        {$$ = $2;printf(";!F\npop rax\nxor rax,1\npush rax\n");} 
  |  '(' Exp ')'                  {$$ = $2;}
  |  LValue                       {if(status == GLOBAL) {$$ = glo_lookup($<ident>1);printf("push QWORD [rbp - %d] ;%s\n",glo_get_addr($<ident>1),$<ident>1);}
                                  else {$$ = loc_lookup($<ident>1); printf("push QWORD [rbp - %d] ;%s\n",loc_get_addr($<ident>1),$<ident>1);}}
  |  NUM                          {$$ = INT; printf("push %d\n",$1);} // on stocke les types pour l'analyse sémantique
  |  CARACTERE                    {$$ = CHAR; printf("push %d\n",$1);}
  |  IDENT '(' Arguments  ')'     {$$ = INT;} //tableau d'entiers uniquement
  ;
LValue:
     IDENT                        {if(status == GLOBAL) {glo_lookup($<ident>1);}
                                  else {loc_lookup($<ident>1);}}
  |  IDENT  '[' Exp  ']'          {if(status == GLOBAL) {glo_lookup($<ident>1);}
                                  else {loc_lookup($<ident>1);}}
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

