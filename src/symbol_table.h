/**
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXNAME 32
#define MAXSYMBOLS 256
#define MAXFUNCTIONS 256

typedef enum scope {
  GLOBAL,
  LOCAL
} Scope;

typedef enum type {
  INT,
  CHAR,
  VOID_T,
  TAB
} Type;

typedef struct {
  char name[MAXNAME];
  int type;
  int addr;
  bool read_only;
} STentry;

typedef struct {
  STentry entries[MAXSYMBOLS];
  int maxsize;
  int size;
} SymbolTable;

typedef struct {
  char name[MAXNAME];
  int return_type;
  int nb_parameters;
} FTentry;

typedef struct {
  FTentry entries[MAXFUNCTIONS];
  int maxsize;
  int size;
} FunctionTable;

void fun_add(const char name[], int rt_type, int nb_par);
void fun_display_table();
int fun_lookup(const char name[], int nb_param);
void glo_addVar(const char name[], int type);
void glo_addConst(const char name[]);
int glo_lookup(const char name[]);
int glo_get_addr(const char name[]);
void glo_display_table();

void loc_addVar(const char name[], int type);
void loc_addConst(const char name[]);
int loc_lookup(const char name[]);
int loc_get_addr(const char name[]);
void loc_display_table();
void loc_clean_table();

void check_expected_type(Type type_to_check, Type type_expected);
void check_expected_types(Type type_to_check, Type type_expected1, Type type_expected2);
bool is_read_only(const char name[], Scope scope);

#endif
