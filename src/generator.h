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

void prologue();
void const_declaration();
void declaration(const char name[], int type, Scope scope);

#endif
