/**
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#include "generator.h"
#include <stdio.h>
#include "symbol_table.h"

void prologue() {
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

void const_declaration() {
  printf("mov rax,60 \n");
  printf("mov rdi,0 \n");
  printf("syscall \n");
  printf(";global table\n");
  glo_display_table();
  printf(";local table\n");
  loc_display_table();
}

void declaration(const char name[], int type, Scope scope) {
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


