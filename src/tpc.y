%{
/*
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 *
 *	TODO :
 *  ------
 * - arrays
 *
 */

int nb_globals = 0;
#include "symbol_table.h"
#include <stdio.h>
#include <getopt.h>
#include "generator.h"

extern int lineno;
int yylex();
void yyerror(char *);
static Scope scope = GLOBAL;
static Type return_type = VOID_T;
static int bss_done = 0;
static int num_label = 0;
static int num_if = 0;
static int nb_param[255];
static int num_scope = -1;
static char fname[64];
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

%type <num> Exp EB TB FB M E T F Parametres
%type <ident> LValue

%left ','
%precedence ')'
%precedence ELSE

%%
Prog:                               { gen_prologue(); }
  DeclConsts DeclVars DeclFoncts    { gen_const_declaration(); }
;
DeclConsts:
  DeclConsts CONST ListConst ';'
|
;
ListConst:
  ListConst ',' IDENT '=' Litteral  { gen_const($<ident>1, $<num>3, scope); }
| IDENT '=' Litteral                { gen_const($<ident>1, $<num>3, scope); }
;
Litteral:
  NombreSigne                       { $<num>$ = $<num>1; }
| CARACTERE                         { $<num>$ = $<num>1; }
;
NombreSigne:
  NUM                               { $<num>$ = $<num>1; }
| ADDSUB NUM                        { $<num>$ = $<addsub>1 == '-' ? - $<num>2 : $<num>2; }
;
DeclVars:
  DeclVars TYPE Declarateurs ';'
|
;
Declarateurs:
  Declarateurs ',' Declarateur      { gen_declaration($<ident>3, $<type>0, scope); }
| Declarateur                       { gen_declaration($<ident>1, $<type>0, scope); }
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
  Corps       { gen_function_end_declaration(fname,return_type,nb_param[num_scope]); scope = GLOBAL; return_type = VOID_T; num_scope--; loc_clean_table(); }
;
EnTeteFonct:
  TYPE IDENT PrologueCont {strcpy(fname,$<ident>2); return_type = gen_function_declaration($<ident>2, $<type>1);} '(' Parametres ')' { nb_param[++num_scope] = $<num>6 ;  scope = GLOBAL;}
| VOID IDENT PrologueCont {strcpy(fname,$<ident>2); return_type = gen_function_declaration($<ident>2, VOID_T);} '(' Parametres ')' { nb_param[++num_scope] = $<num>6 ; scope = GLOBAL; }
;

PrologueCont: {scope = LOCAL;gen_prologue_continue(&bss_done);};

Parametres:
  VOID {$$ = 0;}
| ListTypVar { $<num>$ = $<num>1;}
;
ListTypVar:
  ListTypVar ',' TYPE IDENT { gen_declaration($<ident>4, $<type>3, scope); $<num>$ = $<num>1+1;  }
| TYPE IDENT                { gen_declaration($<ident>2, $<type>1, scope); $<num>$ = 1; }
;
Corps:
  '{' {int i; for(i=1;i<=nb_param[num_scope];i++){fprintf(output, "mov r8, [rbp + %d]\nmov [rbp - %d], r8\n", (nb_param[num_scope]-i+2)*8, i*8);} } DeclConsts DeclVars SuiteInstr '}'
;
SuiteInstr:
  SuiteInstr Instr
|
;
Instr:
  Exp ';'
| ';'
| RETURN Exp ';'                 { gen_function_return(return_type, $<type>2); }
| RETURN ';'                     { gen_function_return(return_type, VOID_T);}
| READE '(' IDENT ')' ';'        { gen_reade($<ident>3); }
| READC '(' IDENT ')' ';'        { gen_readc($<ident>3); }
| PRINT '(' Exp ')' ';'          { gen_print($<type>3);}
| IF '(' Exp IfHandling')' Instr { gen_if_label($<num>4); }
| IF '(' Exp IfHandling')' Instr ELSE IfEndHandling Instr IfElseEndHandling
| WHILE '(' Exp ')' Instr
| '{' SuiteInstr '}'
;
IfHandling:                       { gen_if_start($<num>$ = num_if++); };
IfEndHandling:                    { gen_if_end($<num>-3); };
IfElseEndHandling:                { gen_ifelse_end($<num>-5); };
Exp:
  LValue '=' Exp                { $$ = gen_assign($<ident>1, scope); }
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
  ADDSUB F                      { $$ = gen_signed_expr($1, $2); }
| '!' F                         { $$ = gen_negate_expr($2); }
| '(' Exp ')'                   { $$ = $2; }
| LValue                        { $$ = gen_value($<ident>1, scope); }
| NUM                           { $$ = gen_num($1, scope); }
| CARACTERE                     { $$ = gen_char($1, scope); }
| IDENT '(' Arguments  ')'      { $$ = gen_function_call($<ident>1,$<num>3); } 
;
LValue:
  IDENT                        { gen_check($<ident>1, scope); }
| IDENT '[' Exp ']'            { gen_check($<ident>1, scope); }
;
Arguments:
  ListExp
| {$<num>$ = 0;}
;
ListExp:
  ListExp ',' Exp {$<num>$ = $<num>1 + 1;}
| Exp {$<num>$ = 1;}
;
%%

void yyerror(char *msg){
  fprintf(stderr, "%s at line %d\n", msg, lineno);
}

int main(int argc, char **argv) {
  char opt;

  output = stdout;
  while ((opt = getopt(argc, argv, "o:")) != -1)
    switch (opt) {
    case 'o':
      output = fopen(optarg, "w+");
      if (output == NULL) {
        perror("Error opening output file: ");
        return -1;
      }
      break;

    default:
      fputs("Exiting due to invalid option.\n", stderr);
      return -1;
    }

  return yyparse();
}
