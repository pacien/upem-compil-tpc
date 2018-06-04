/**
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#include "generator.h"
#include "symbol_table.h"

// ----- GLOBAL FUNCTIONS -----

void gen_prologue() {
  fprintf(output, "extern printf\nextern scanf\n");
  fprintf(output, "section .data\n");
  fprintf(output, "format_int db \"%%d\",10,0\n");
  fprintf(output, "format_char db \"%%c\",10,0\n");
  fprintf(output, "format_int_in: db \"%%d\", 0\n");
  fprintf(output, "format_char_in: db \"%%c\", 0\n\n");
  fprintf(output, "section .bss\n");
}
void gen_prologue_continue(int *bss_done) {
  if (*bss_done == 0) {
    fprintf(output, "globals: resq %d\n", nb_globals);
    fprintf(output, "section .text\n\nglobal _start\n");
    fprintf(output, "\nprinte: ;print needs an argument in rax\n");
    fprintf(output, "push rbp\n");
    fprintf(output, "mov rbp, rsp\n");
    fprintf(output, "push rsi\n");
    fprintf(output, "push rdi\n");
    fprintf(output, "mov rsi, rax\n");
    fprintf(output, "mov rdi, format_int\n");
    fprintf(output, "mov rax, 0\n");
    fprintf(output, "call printf WRT ..plt\n");
    fprintf(output, "pop rdi\n");
    fprintf(output, "pop rsi\n");
    fprintf(output, "pop rbp\n");
    fprintf(output, "ret\n");
    fprintf(output, "\nprintc: ;print needs an argument in rax\n");
    fprintf(output, "push rbp\n");
    fprintf(output, "mov rbp, rsp\n");
    fprintf(output, "push rsi\n");
    fprintf(output, "push rdi\n");
    fprintf(output, "mov rsi, rax\n");
    fprintf(output, "mov rdi, format_char\n");
    fprintf(output, "mov rax, 0\n");
    fprintf(output, "call printf WRT ..plt\n");
    fprintf(output, "pop rdi\n");
    fprintf(output, "pop rsi\n");
    fprintf(output, "pop rbp\n");
    fprintf(output, "ret\n");
    fprintf(output, "\nreade: ;read needs an adress in rax\n");
    fprintf(output, "push rbp\n");
    fprintf(output, "mov rbp, rsp\n");
    fprintf(output, "push rsi\n");
    fprintf(output, "push rdi\n");
    fprintf(output, "mov rsi, rax\n");
    fprintf(output, "mov rdi, format_int_in\n");
    fprintf(output, "mov rax, 0\n");
    fprintf(output, "call scanf\n");
    fprintf(output, "pop rdi\n");
    fprintf(output, "pop rsi\n");
    fprintf(output, "pop rbp\n");
    fprintf(output, "ret\n");
    fprintf(output, "\nreadc: ;read needs an adress in rax\n");
    fprintf(output, "push rbp\n");
    fprintf(output, "mov rbp, rsp\n");
    fprintf(output, "push rsi\n");
    fprintf(output, "push rdi\n");
    fprintf(output, "mov rsi, rax\n");
    fprintf(output, "mov rdi, format_char_in\n");
    fprintf(output, "mov rax, 0\n");
    fprintf(output, "call scanf\n");
    fprintf(output, "pop rdi\n");
    fprintf(output, "pop rsi\n");
    fprintf(output, "pop rbp\n");
    fprintf(output, "ret\n");
    *bss_done = 1;
  }
}
void gen_const_declaration() {
  fprintf(output, "\n_start:\n");
  fprintf(output, "push rbp\nmov rbp, rsp\n\n");
  fprintf(output, "call main\n");
  fprintf(output, "\nmov rax,60 \n");
  fprintf(output, "mov rdi,0 \n");
  fprintf(output, "syscall \n\n");
  fprintf(output, ";global table\n");
  glo_display_table();
  fprintf(output, ";local table\n");
  loc_display_table();
  fprintf(output, ";function table\n");
  fun_display_table();
}

void gen_function_declaration(const char name[], int return_type,
                              int nb_param) {
  fun_add(name, return_type, nb_param);
  fprintf(output, "\n%s:\npush rbp\nmov rbp,rsp\n", name);
}

void gen_function_end_declaration() {
  fprintf(output, "mov rsp, rbp\npop rbp\nret\n");
}
int gen_function_call(const char name[], int nb_param) {
  fprintf(output, "call %s\n", name);
  return fun_lookup(name, nb_param);
}

void gen_declaration(const char name[], int type, Scope scope) {
  switch (scope) {
  case GLOBAL:
    glo_addVar(name, type);
    nb_globals++;
    break;
  case LOCAL:
    loc_addVar(name, type);
    fprintf(output, "push 0\n");
    break;
  }
}

// Verifies that the variable exists
void gen_check(const char name[], Scope scope) {
  switch (scope) {
  case GLOBAL:
    glo_lookup(name);
    break;
  case LOCAL:
    loc_lookup(name);
    break;
  }
}

// ----- READ AND PRINT FUNCTIONS -----
void gen_reade(const char name[]) {
  if (loc_lookup(name) != INT) {
    fprintf(stderr, "Need to be a INT in the reade() function\n");
    return;
  }
  int l_addr = loc_get_addr(name);
  int g_addr = glo_get_addr(name);
  if (l_addr != -1) {
    fprintf(output, "mov rax,rbp\nsub rax,%d\ncall reade\n",
            loc_get_addr(name));
    return;
  }
  fprintf(output, "mov rax,globals\nadd rax,%d\ncall reade\n", g_addr);
}

void gen_readc(const char name[]) {
  if (loc_lookup(name) != CHAR) {
    fprintf(stderr, "Need to be a CHAR in the readc() function\n");
    return;
  }
  int l_addr = loc_get_addr(name);
  int g_addr = glo_get_addr(name);
  if (l_addr != -1) {
    fprintf(output, "mov rax,rbp\nsub rax,%d\ncall readc\n",
            loc_get_addr(name));
    return;
  }
  fprintf(output, "mov rax,globals\nadd rax,%d\ncall readc\n", g_addr);
}

void gen_print(int type) {
  //check if the name exists in both tables
  fprintf(output, "pop rax\n");
  switch(type){
    case INT:
      fprintf(output, "call printe\n");
    break;
    case CHAR:
      fprintf(output, "call printc\n");
    break;
    default: fprintf(stderr, "Error print, supposed to have type CHAR or INT");
  }
  
}

// ----- CONDITIONAL BRANCHING ------

void gen_if_label(int idx) {
  fprintf(output, ".end_if%d:\n", idx);
  fprintf(output, ";ENDIF\n\n");
}

void gen_if_start(int idx) {
  fprintf(output, "\n;BEGINIF\n");
  fprintf(output, "pop rax\n");
  fprintf(output, "cmp rax,0\n");
  fprintf(output, "jz .end_if%d\n", idx);
}

void gen_if_end(int idx) {
  fprintf(output, "jmp .end_ifelse%d\n", idx);
  fprintf(output, ".end_if%d:\n", idx);
}

void gen_ifelse_end(int idx) {
  fprintf(output, ".end_ifelse%d:\n", idx);
  fprintf(output, "ENDIF\n\n");
}

// ----- OPERATORS -----

int gen_assign(const char ident[], Scope scope) {
  int l_addr = loc_get_addr(ident);
  int g_addr = glo_get_addr(ident);
  switch (scope) {
  case GLOBAL:
    fprintf(output, "pop QWORD [globals + %d] ;%s\n", g_addr,
            ident);
    return glo_lookup(ident);
  case LOCAL:
    if (l_addr != -1) {
      fprintf(output, "pop QWORD [rbp - %d] ;%s\n", l_addr, ident);
      return loc_lookup(ident);
    } else {
      fprintf(output, "pop QWORD [globals + %d] ;%s\n", g_addr, ident);
      return glo_lookup(ident);
    }
  default:
    exit(1);
  }
}

void gen_or(int left, int right, int idx) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  fprintf(output, ";a OR c\n");
  fprintf(output, "pop rax\n");
  fprintf(output, "cmp rax,1\n");
  fprintf(output, "je .true%d\n", idx);
  fprintf(output, "pop rcx\n");
  fprintf(output, "cmp rcx,1\n");
  fprintf(output, "jz .true%d\n", idx);
  fprintf(output, "push 0\n");
  fprintf(output, "jmp .false%d\n", idx);
  fprintf(output, ".true%d:\n", idx);
  fprintf(output, "push 1\n");
  fprintf(output, ".false%d:", idx);
}

void gen_and(int left, int right, int idx) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  fprintf(output, ";a AND c\n");
  fprintf(output, "pop rax\n");
  fprintf(output, "cmp rax,0\n");
  fprintf(output, "jz .false%d\n", idx);
  fprintf(output, "pop rcx\n");
  fprintf(output, "cmp rcx,0\n");
  fprintf(output, "jz .false%d\n", idx);
  fprintf(output, "push 1\n");
  fprintf(output, "jmp .true%d\n", idx);
  fprintf(output, ".false%d:\n", idx);
  fprintf(output, "push 0\n");
  fprintf(output, ".true%d:", idx);
}

void gen_eq(const char op[], int left, int right, int idx) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  fprintf(output, ";a EQ c\npop rax\npop rcx\ncmp rax,rcx\n");

  if (!strcmp(op, "=="))
    fprintf(output, "je .true%d\n", idx);
  else if (!strcmp(op, "!="))
    fprintf(output, "jne .true%d\n", idx);
  else
    exit(1); // TODO: error on unexpected op

  fprintf(output, "push 0\njmp .false%d\n.true%d:\npush 1\n.false%d:", idx, idx,
          idx);
}

void gen_order(const char op[], int left, int right, int idx) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  fprintf(output, ";a ORDER c\npop rcx\npop rax\ncmp rax,rcx\n");

  if (!strcmp(op, "<"))
    fprintf(output, "jl .true%d\n", idx);
  else if (!strcmp(op, "<="))
    fprintf(output, "jle .true%d\n", idx);
  else if (!strcmp(op, ">"))
    fprintf(output, "jg .true%d\n", idx);
  else if (!strcmp(op, "<="))
    fprintf(output, "jge .true%d\n", idx);
  else
    exit(1); // TODO: error on unexpected op

  fprintf(output, "push 0\njmp .false%d\n.true%d:\npush 1\n.false%d:", idx, idx,
          idx);
}

void gen_addsub(char op, int left, int right) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  switch (op) {
  case '+':
    fprintf(output, ";E + T\npop rcx\npop rax\nadd rax,rcx\npush rax\n");
    break;
  case '-':
    fprintf(output, ";E - T\npop rcx\npop rax\nsub rax,rcx\npush rax\n");
    break;
  default:
    exit(1);
  }
}

void gen_divstar(char op, int left, int right) {
  check_expected_type(left, INT);
  check_expected_type(right, INT);

  switch (op) {
  case '*':
    fprintf(output, ";E * T\npop rax\npop rcx\nimul rax,rcx\npush rax\n");
    break;
  case '/':
  case '%':
    fprintf(output, ";E / T\npop rax\npop rcx\nxor rdx,rdx\nidiv rcx\n");
    fprintf(output, "push %s\n", op == '/' ? "rax" : "rdx");
    break;
  default:
    exit(1);
  }
}

int gen_signed_expr(char op, int type) {
  switch (op) {
  case '+':
    fprintf(output, ";+F\n");
    break;
  case '-':
    fprintf(output, ";-F\npop rdx\nxor rax,rax\nsub rax,rdx\npush rax\n");
    break;
  default:
    exit(1);
  }

  return type;
}

int gen_negate_expr(int type) {
  fprintf(output, ";!F\npop rax\nxor rax,1\npush rax\n");
  return type;
}

int gen_value(const char ident[], Scope scope) {
  int l_addr = loc_get_addr(ident);
  int g_addr = glo_get_addr(ident);
  switch (scope) {
  case GLOBAL:
    fprintf(output, "push QWORD [globals + %d] ;%s\n", g_addr,
            ident);
    return glo_lookup(ident);
  case LOCAL:
    if (l_addr != -1) {
      fprintf(output, "push QWORD [rbp - %d] ;%s\n", l_addr,
              ident);
      return loc_lookup(ident);
    } else {
      fprintf(output, "push QWORD [globals + %d] ;%s\n", g_addr, ident);
      return glo_lookup(ident);
    }
  default:
    exit(1);
  }
}

int gen_num(int value, Scope scope) {
  if (scope == LOCAL)
    fprintf(output, "push %d\n", value); // TODO: remove if?
  // stored for the semantic analysis.

  return INT;
}

int gen_char(int value, Scope scope) {
  if (scope == LOCAL)
    fprintf(output, "push %d\n", value); // TODO: remove if?
  return CHAR;
}
