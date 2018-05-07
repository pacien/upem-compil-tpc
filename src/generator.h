/**
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#ifndef __GENERATOR_H__
#define __GENERATOR_H__

typedef enum scope {
  GLOBAL,
  LOCAL
} Scope;

void gen_prologue();
void gen_const_declaration();
void gen_declaration(const char name[], int type, Scope scope);

void gen_read(const char name[], Scope scope);
void gen_print();

void gen_if_label(int idx);
void gen_if_start(int idx);
void gen_if_end(int idx);
void gen_ifelse_end(int idx);

void gen_assign(int ident, Scope scope);
void gen_or(int left, int right, int idx);
void gen_and(int left, int right, int idx);
void gen_eq(const char op[], int left, int right, int idx);
void gen_order(const char op[], int left, int right, int idx);
void gen_addsub(char op, int left, int right);
void gen_divstar(char op, int left, int right);

#endif
