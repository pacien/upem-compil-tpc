/**
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#include "generator.h"
#include <stdio.h>
#include "symbol_table.h"

// ----- GLOBAL FUNCTIONS -----

void gen_prologue() {
  printf("extern printf\n");
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

void gen_const_declaration() {
  printf("mov rax,60 \n");
  printf("mov rdi,0 \n");
  printf("syscall \n");
  printf(";global table\n");
  glo_display_table();
  printf(";local table\n");
  loc_display_table();
}

void gen_declaration(const char name[], int type, Scope scope) {
  switch (scope) {
  case GLOBAL:
    glo_addVar(name, type);
    break;
  case LOCAL:
    loc_addVar(name, type);
    break;
  }

  printf("push 0\n");
}

// ----- READ AND PRINT FUNCTIONS -----

void gen_read(const char name[], Scope scope) {
  switch (scope) {
  case GLOBAL:
    glo_lookup(name);
    break;
  case LOCAL:
    loc_lookup(name);
    break;
  }
}

void gen_print() {
  printf("pop rax\n");
  printf("call print\n");
}

// ----- CONDITIONAL BRANCHING ------

void gen_if_label(int idx) {
  printf(".end_if%d:\n", idx);
  printf(";ENDIF\n\n");
}

void gen_if_start(int idx) {
  printf("\n;BEGINIF\n");
  printf("pop rax\n");
  printf("cmp rax,0\n");
  printf("jz .end_if%d\n", idx);
}

void gen_if_end(int idx) {
  printf("jmp .end_ifelse%d\n", idx);
  printf(".end_if%d:\n", idx);
}

void gen_ifelse_end(int idx) {
  printf(".end_ifelse%d:\n", idx);
  printf("ENDIF\n\n");
}

// ----- OPERATORS -----

int gen_assign(const char ident[], Scope scope) {
  const char instr_fmt[] = "pop QWORD [rbp - %d] ;%s\n";

  switch (scope) {
  case GLOBAL:
    printf(instr_fmt, glo_get_addr(ident), ident);
    return glo_lookup(ident);
  case LOCAL:
    printf(instr_fmt, loc_get_addr(ident), ident);
    return loc_lookup(ident);
  default:
    exit(1);
  }
}

void gen_or(int left, int right, int idx) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  printf(";a OR c\n");
  printf("pop rax\n");
  printf("cmp rax,1\n");
  printf("je .true%d\n", idx);
  printf("pop rcx\n");
  printf("cmp rcx,1\n");
  printf("jz .true%d\n", idx);
  printf("push 0\n");
  printf("jmp .false%d\n", idx);
  printf(".true%d:\n", idx);
  printf("push 1\n");
  printf(".false%d:", idx);
}

void gen_and(int left, int right, int idx) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  printf(";a AND c\n");
  printf("pop rax\n");
  printf("cmp rax,0\n");
  printf("jz .false%d\n", idx);
  printf("pop rcx\n");
  printf("cmp rcx,0\n");
  printf("jz .false%d\n", idx);
  printf("push 1\n");
  printf("jmp .true%d\n", idx);
  printf(".false%d:\n", idx);
  printf("push 0\n");
  printf(".true%d:", idx);
}

void gen_eq(const char op[], int left, int right, int idx) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  printf(";a EQ c\npop rax\npop rcx\ncmp rax,rcx\n");

  if (!strcmp(op, "=="))
    printf("je .true%d\n", idx);
  else if (!strcmp(op, "!="))
    printf("jne .true%d\n", idx);
  else
    exit(1); // TODO: error on unexpected op

  printf("push 0\njmp .false%d\n.true%d:\npush 1\n.false%d:", idx, idx, idx);
}

void gen_order(const char op[], int left, int right, int idx) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  printf(";a ORDER c\npop rcx\npop rax\ncmp rax,rcx\n");

  if (!strcmp(op, "<"))
    printf("jl .true%d\n", idx);
  else if (!strcmp(op, "<="))
    printf("jle .true%d\n", idx);
  else if(!strcmp(op, ">"))
    printf("jg .true%d\n", idx);
  else if(!strcmp(op, "<="))
    printf("jge .true%d\n", idx);
  else
    exit(1); // TODO: error on unexpected op

  printf("push 0\njmp .false%d\n.true%d:\npush 1\n.false%d:", idx, idx, idx);
}

void gen_addsub(char op, int left, int right) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  switch (op) {
  case '+':
    printf(";E + T\npop rcx\npop rax\nadd rax,rcx\npush rax\n");
    break;
  case '-':
    printf(";E - T\npop rcx\npop rax\nsub rax,rcx\npush rax\n");
    break;
  default:
    exit(1);
  }
}

void gen_divstar(char op, int left, int right) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  switch (op) {
  case '*':
    printf(";E * T\npop rax\npop rcx\nimul rax,rcx\npush rax\n");
    break;
  case '/':
  case '%':
    printf(";E / T\npop rax\npop rcx\nxor rdx,rdx\nidiv rcx\n");
    printf("push %s\n", op == '/' ? "rax" : "rdx");
    break;
  default:
    exit(1);
  }
}

int gen_signed_expr(char op, int type) {
  switch (op) {
  case '+':
    printf(";+F\n");
    break;
  case '-':
    printf(";-F\npop rdx\nxor rax,rax\nsub rax,rdx\npush rax\n");
    break;
  default:
    exit(1);
  }

  return type;
}

int gen_negate_expr(int type) {
  printf(";!F\npop rax\nxor rax,1\npush rax\n");
  return type;
}

int gen_value(const char ident[], Scope scope) {
  switch (scope) {
  case GLOBAL:
    printf("push QWORD [rbp - %d] ;%s\n", glo_get_addr(ident), ident);
    return glo_lookup(ident);
  case LOCAL:
    printf("push QWORD [rbp - %d] ;%s\n", loc_get_addr(ident), ident);
    return loc_lookup(ident);
  default:
    exit(1);
  }
}

int gen_num(int value, Scope scope) {
  if (scope == LOCAL) printf("push %d\n", value); // TODO: remove if?
  // stored for the semantic analysis.

  return INT;
}

int gen_char(int value, Scope scope) {
  if(scope == LOCAL) printf("push %d\n", value); // TODO: remove if?
  return CHAR;
}

