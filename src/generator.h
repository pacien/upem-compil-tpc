/**
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <stdio.h>

typedef enum scope {
  GLOBAL,
  LOCAL
} Scope;

extern int nb_globals;
FILE *output;

void gen_prologue();
void gen_prologue_continue(int *bss_done);
void gen_const_declaration();
void gen_function_declaration(const char name[], int return_type, int nb_param);
void gen_function_end_declaration();
int gen_function_call(const char name[], int nb_param);
void gen_declaration(const char name[], int type, Scope scope);

void gen_read(const char name[], Scope scope);
void gen_print();

void gen_if_label(int idx);
void gen_if_start(int idx);
void gen_if_end(int idx);
void gen_ifelse_end(int idx);

int gen_assign(const char ident[], Scope scope);
void gen_or(int left, int right, int idx);
void gen_and(int left, int right, int idx);
void gen_eq(const char op[], int left, int right, int idx);
void gen_order(const char op[], int left, int right, int idx);
void gen_addsub(char op, int left, int right);
void gen_divstar(char op, int left, int right);

int gen_signed_expr(char op, int type);
int gen_negate_expr(int type);
int gen_value(const char ident[], Scope scope);
int gen_num(int value, Scope scope);
int gen_char(int value, Scope scope);

#endif
