#include "symboltable.h"

extern int lineno; /* from lexical analyser */

SymbolTable glo_symbol_table = {{{{0},0}},MAXSYMBOLS,0};
SymbolTable loc_symbol_table = {{{{0},0}},MAXSYMBOLS,0};

void glo_addVar(const char name[], int type) {
  int count;
  for (count = 0; count < glo_symbol_table.size; count++) {
    if (!strcmp(glo_symbol_table.entries[count].name, name)) {
      fprintf(stderr,"semantic error, redefinition of variable %s near line %d\n", name,
             lineno);
      return;
    }
  }
  if (++glo_symbol_table.size > glo_symbol_table.maxsize) {
    fprintf(stderr,"too many variables near line %d\n", lineno);
    exit(1);
  }
  strcpy(glo_symbol_table.entries[glo_symbol_table.size - 1].name, name);
  glo_symbol_table.entries[glo_symbol_table.size - 1].type = type;
  glo_symbol_table.entries[glo_symbol_table.size - 1].addr = (glo_symbol_table.size - 1)*8;
}

//Verifies that the variable exists and returns the type
int glo_lookup(const char name[]) {
  int count;

  for (count = 0; count < glo_symbol_table.size; count++) {
    if (!strcmp(glo_symbol_table.entries[count].name, name)) {
      return glo_symbol_table.entries[count].type;
    }
  }
  fprintf(stderr,"No definition of the variable %s near line %d\n", name,
           lineno);
  return -1;
}


void glo_display_table(){
    int count;
    for (count=0;count<glo_symbol_table.size;count++) {
        if(glo_symbol_table.entries[count].type == INT)
            printf("entier: %s, pos: %d     \n", glo_symbol_table.entries[count].name, glo_symbol_table.entries[count].addr);
        else
            printf("caractere: %s, pos: %d       \n", glo_symbol_table.entries[count].name, glo_symbol_table.entries[count].addr);
    }
    printf("\n");
}


void loc_addVar(const char name[], int type) {
  int count;
  for (count = 0; count < loc_symbol_table.size; count++) {
    if (!strcmp(loc_symbol_table.entries[count].name, name)) {
      fprintf(stderr,"semantic error, redefinition of variable %s near line %d\n", name,
             lineno);
      return;
    }
  }
  if (++glo_symbol_table.size > glo_symbol_table.maxsize) {
    fprintf(stderr,"too many variables near line %d\n", lineno);
    exit(1);
  }
  strcpy(glo_symbol_table.entries[glo_symbol_table.size - 1].name, name);
  glo_symbol_table.entries[glo_symbol_table.size - 1].type = type;
  glo_symbol_table.entries[glo_symbol_table.size - 1].addr = (glo_symbol_table.size - 1)*8;
}


int loc_lookup(const char name[]) {
  int count;

  for (count = 0; count < loc_symbol_table.size; count++) {
    if (!strcmp(loc_symbol_table.entries[count].name, name)) {
      return loc_symbol_table.entries[count].type;
    }
  }
  fprintf(stderr,"No definition of the variable %s near line %d\n", name,
           lineno);
  return -1;
}
void loc_display_table(){
    int count;
    for (count=0;count<loc_symbol_table.size;count++) {
        if(loc_symbol_table.entries[count].type == INT)
            printf("entier: %s, pos: %d     \n", loc_symbol_table.entries[count].name, loc_symbol_table.entries[count].addr);
        else
            printf("caractere: %s, pos: %d       \n", loc_symbol_table.entries[count].name, loc_symbol_table.entries[count].addr);
    }
    printf("\n");
}