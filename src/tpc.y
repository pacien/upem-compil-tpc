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
Prog:{printf("section .data\n");
			printf("format_entier db \"%%d \n\", 10,0");
			printf("section .bss\nsection .text\nglobal _start\n");
			printf("print:\n");
			printf("push rbp\n");
			printf("mov rbp, rsp\n");
			
			printf("push rax\n");
			printf("push rcx\n");
			printf("push rdx\n");
			printf("push rdi\n");
			printf("push rsi\n");
			printf("push r8\n");
			
			printf("mov r8, rdx \n");
			printf("mov rcx, rcx\n");
			printf("mov rdx, rbx\n");
		  printf("  mov rdi, format_entier\n");
		  printf("  mov rax, 0\n");
		  printf("  call printf WRT ..plt\n");	
	
			printf("pop r8\n");
			printf("pop rsi\n");
			printf("pop rdi\n");
			printf("pop rdx\n");
			printf("pop rcx\n");
			printf("pop rax\n");
			
			printf("pop rbp\n");
			printf("ret\n");	
			}
     DeclConsts DeclVars DeclFoncts {
     		
     	glo_display_table();}
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
     Declarateurs ',' Declarateur   {glo_addVar($<ident>3, $<type>0);}
  |  Declarateur                    {glo_addVar($<ident>1, $<type>0);}
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
     ListTypVar ',' TYPE IDENT    {glo_addVar($<ident>4, $<type>3);}
  |  TYPE IDENT                   {glo_addVar($<ident>2, $<type>1);}
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
  |  READE '(' IDENT ')' ';'      {glo_lookup($<ident>3);}
  |  READC '(' IDENT ')' ';'      {glo_lookup($<ident>3);}
  |  PRINT '(' Exp ')' ';'
  |  IF '(' Exp ')' Instr
  |  IF '(' Exp ')' Instr ELSE Instr
  |  WHILE '(' Exp ')' Instr
  |  '{' SuiteInstr '}'
  ;
Exp:
     LValue '=' Exp               {$$ = glo_lookup($<ident>1);}
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
  |  LValue                     {$$ = glo_lookup($<ident>1);}
  |  NUM                        {$$ = INT;} // on stocke les types pour l'analyse sémantique
  |  CARACTERE                  {$$ = CHAR;}
  |  IDENT '(' Arguments  ')'   {$$ = INT;} //tableau d'entiers uniquement
  ;
LValue:
     IDENT                      {glo_lookup($<ident>1);}
  |  IDENT  '[' Exp  ']'        {glo_lookup($<ident>1);}
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

