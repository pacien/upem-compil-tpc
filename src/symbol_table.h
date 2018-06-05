/**
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNAME 32
#define MAXSYMBOLS 256
#define MAXFUNCTIONS 256

typedef enum type {
  INT,
  CHAR,
  VOID_T
} Type;

typedef struct {
  char name[MAXNAME];
  int type;
  int addr;
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
int glo_lookup(const char name[]);
int glo_get_addr(const char name[]);
void glo_display_table();
void loc_addVar(const char name[], int type);
int loc_lookup(const char name[]);
int loc_get_addr(const char name[]);
void loc_display_table();
void loc_clean_table();
void check_expected_type(int type_to_check, int type_expected);

#endif
