#include "symboltable.h"

extern int lineno; /* from lexical analyser */

SymbolTable symbol_table = {{{{0},0}},MAXSYMBOLS,0};

void addVar(const char name[], int type) {
  int count;
  for (count = 0; count < symbol_table.size; count++) {
    if (!strcmp(symbol_table.entries[count].name, name)) {
      printf("semantic error, redefinition of variable %s near line %d\n", name,
             lineno);
      return;
    }
  }
  if (++symbol_table.size > symbol_table.maxsize) {
    printf("too many variables near line %d\n", lineno);
    exit(1);
  }
  strcpy(symbol_table.entries[symbol_table.size - 1].name, name);
  symbol_table.entries[symbol_table.size - 1].type = type;
}

void lookup(const char name[]) {
  int count;

  for (count = 0; count < symbol_table.size; count++) {
    if (!strcmp(symbol_table.entries[count].name, name)) {
      return;
    }
    printf("No definition of the variable %s near line %d\n", name,
           lineno);
  }
}

