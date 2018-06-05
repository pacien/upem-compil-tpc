/**
  * UPEM / Compilation / Projet
  * Pacien TRAN-GIRARD, Adam NAILI
  */

#include "symbol_table.h"
#include "generator.h"

extern int lineno; /* from lexical analyser */

SymbolTable glo_symbol_table = {{{{0}, 0}}, MAXSYMBOLS, 0};
SymbolTable loc_symbol_table = {{{{0}, 0}}, MAXSYMBOLS, 0};
FunctionTable fun_table = {{{{0}, 0}}, MAXFUNCTIONS, 0};

void fun_add(const char name[], int rt_type, int nb_par) {
  int count;
  for (count = 0; count < fun_table.size; count++) {
    if (!strcmp(fun_table.entries[count].name, name)) {
      fprintf(stderr,
              "semantic error, redefinition of function %s near line %d\n",
              name, lineno);
      return;
    }
  }
  if (++fun_table.size > fun_table.maxsize) {
    fprintf(stderr, "too many functions near line %d\n", lineno);
    exit(1);
  }
  strcpy(fun_table.entries[fun_table.size - 1].name, name);
  fun_table.entries[fun_table.size - 1].return_type = rt_type;
  fun_table.entries[fun_table.size - 1].nb_parameters = nb_par;
}

void fun_display_table() {
  int count;
  for (count = 0; count < fun_table.size; count++) {
    if (fun_table.entries[count].return_type == INT)
      fprintf(output,
              ";%s, type de retour : entier, nombre de parametres : %d \n",
              fun_table.entries[count].name,
              fun_table.entries[count].nb_parameters);
    else if (fun_table.entries[count].return_type == CHAR)
      fprintf(output,
              ";%s, type de retour : caractere, nombre de parametres : %d \n",
              fun_table.entries[count].name,
              fun_table.entries[count].nb_parameters);
    else
      fprintf(output,
              ";%s, type de retour : void, nombre de parametres : %d \n",
              fun_table.entries[count].name,
              fun_table.entries[count].nb_parameters);
  }
  fprintf(output, "\n");
}

int fun_lookup(const char name[], int nb_param) {
  int count;

  for (count = 0; count < fun_table.size; count++) {
    if (!strcmp(fun_table.entries[count].name, name) && (fun_table.entries[count].nb_parameters == nb_param)) {
      return fun_table.entries[count].return_type;
    }
  }
  fprintf(stderr, "No definition of the function %s (or wrong number of parameters) near line %d\n", name,
          lineno);
  return -1;
}

static void glo_add(const char name[], int type, bool read_only) {
  int count;
  for (count = 0; count < glo_symbol_table.size; count++) {
    if (!strcmp(glo_symbol_table.entries[count].name, name)) {
      fprintf(stderr,
              "semantic error, redefinition of variable %s near line %d\n",
              name, lineno);
      return;
    }
  }
  if (++glo_symbol_table.size > glo_symbol_table.maxsize) {
    fprintf(stderr, "too many variables near line %d\n", lineno);
    exit(1);
  }
  strcpy(glo_symbol_table.entries[glo_symbol_table.size - 1].name, name);
  glo_symbol_table.entries[glo_symbol_table.size - 1].type = type;
  glo_symbol_table.entries[glo_symbol_table.size - 1].addr = (glo_symbol_table.size - 1) * 8;
  glo_symbol_table.entries[glo_symbol_table.size - 1].read_only = read_only;
}

void glo_addVar(const char name[], int type) {
  glo_add(name, type, false);
}

void glo_addConst(const char name[]) {
  glo_add(name, INT, true);
}

// Verifies that the variable exists and returns the type
int glo_lookup(const char name[]) {
  int count;

  for (count = 0; count < glo_symbol_table.size; count++) {
    if (!strcmp(glo_symbol_table.entries[count].name, name)) {
      return glo_symbol_table.entries[count].type;
    }
  }
  fprintf(stderr, "No definition of the variable %s near line %d\n", name,
          lineno);
  return -1;
}

int glo_get_addr(const char name[]) {
  int count;
  for (count = 0; count < glo_symbol_table.size; count++) {
    if (!strcmp(glo_symbol_table.entries[count].name, name)) {
      return glo_symbol_table.entries[count].addr;
    }
  }
  return -1;
}

void glo_display_table() {
  int count;
  for (count = 0; count < glo_symbol_table.size; count++) {
    if (glo_symbol_table.entries[count].type == INT)
      fprintf(output, ";entier: %s, pos: %d     \n",
              glo_symbol_table.entries[count].name,
              glo_symbol_table.entries[count].addr);
    else
      fprintf(output, ";caractere: %s, pos: %d       \n",
              glo_symbol_table.entries[count].name,
              glo_symbol_table.entries[count].addr);
  }
  fprintf(output, "\n");
}

static void loc_add(const char name[], int type, bool read_only) {
  int count;
  for (count = 0; count < loc_symbol_table.size; count++) {
    if (!strcmp(loc_symbol_table.entries[count].name, name)) {
      fprintf(stderr,
              "semantic error, redefinition of variable %s near line %d\n",
              name, lineno);
      return;
    }
  }
  if (++loc_symbol_table.size > loc_symbol_table.maxsize) {
    fprintf(stderr, "too many variables near line %d\n", lineno);
    exit(1);
  }
  strcpy(loc_symbol_table.entries[loc_symbol_table.size - 1].name, name);
  loc_symbol_table.entries[loc_symbol_table.size - 1].type = type;
  loc_symbol_table.entries[loc_symbol_table.size - 1].addr = (loc_symbol_table.size - 1) * 8 + 8;
  loc_symbol_table.entries[loc_symbol_table.size - 1].read_only = read_only;
}

void loc_addVar(const char name[], int type) {
  loc_add(name, type, false);
}

void loc_addConst(const char name[]) {
  loc_add(name, INT, true);
}

int loc_lookup(const char name[]) {
  int count;

  for (count = 0; count < loc_symbol_table.size; count++) {
    if (!strcmp(loc_symbol_table.entries[count].name, name)) {
      return loc_symbol_table.entries[count].type;
    }
  }
  // Check in global table
  for (count = 0; count < glo_symbol_table.size; count++) {
    if (!strcmp(glo_symbol_table.entries[count].name, name)) {
      return glo_symbol_table.entries[count].type;
    }
  }
  fprintf(stderr, "No definition of the variable %s near line %d\n", name,
          lineno);
  return -1;
}

int loc_get_addr(const char name[]) {
  int count;

  for (count = 0; count < loc_symbol_table.size; count++) {
    if (!strcmp(loc_symbol_table.entries[count].name, name)) {
      return loc_symbol_table.entries[count].addr;
    }
  }
  return -1;
}
void loc_display_table() {
  int count;
  for (count = 0; count < loc_symbol_table.size; count++) {
    if (loc_symbol_table.entries[count].type == INT)
      fprintf(output, ";entier: %s, pos: %d     \n",
              loc_symbol_table.entries[count].name,
              loc_symbol_table.entries[count].addr);
    else
      fprintf(output, ";caractere: %s, pos: %d       \n",
              loc_symbol_table.entries[count].name,
              loc_symbol_table.entries[count].addr);
  }
  fprintf(output, "\n");
}

void loc_clean_table() {
  loc_symbol_table.size = 0;
}

static char *string_of_type(int type) {
  switch (type) {
  case INT:
    return "INT";
  case CHAR:
    return "CHAR";
  case VOID_T:
    return "VOID";
  default:
    return "UNEXPECTED";
  }
}

void check_expected_type(int type_to_check, int type_expected) {
  if (type_to_check != type_expected)
    fprintf(stderr, "Expected type : %s -> Got type : %s (near line %d)\n",
            string_of_type(type_expected), string_of_type(type_to_check),
            lineno);
}
