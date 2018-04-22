#ifndef __SYMBOLTABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNAME 32
#define INT 0
#define CHAR 1
#define MAXSYMBOLS 256


typedef struct {
    char name[MAXNAME];
    int type;
} STentry;


typedef struct {
	STentry entries[MAXSYMBOLS];
	int maxsize;
	int size;
} SymbolTable;


void addVar(const char name[], int type);
void lookup(const char name[]);
void display_table();

#endif