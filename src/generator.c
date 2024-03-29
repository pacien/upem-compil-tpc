/**
 * UPEM / Compilation / Projet
 * Pacien TRAN-GIRARD, Adam NAILI
 */

#include "generator.h"
void yyerror(char *);
// ----- GLOBAL FUNCTIONS -----

void gen_prologue() {
  fprintf(output, "extern printf\nextern scanf\n");
  fprintf(output, "section .data\n");
}

void gen_prologue_continue(bool *bss_done) {
  if (*bss_done) return;

  fprintf(output, "format_int db \"%%d\",10,0\n");
  fprintf(output, "format_char db \"%%c\",10,0\n");
  fprintf(output, "format_int_in: db \"%%d\", 0\n");
  fprintf(output, "format_char_in: db \"%%c\", 0\n\n");
  fprintf(output, "section .bss\n");
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
  *bss_done = true;
}

void gen_epilogue() {
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

  // require a main function
  fun_lookup("main", 0);
}

void gen_const(const char name[], int value, Scope scope) {
  switch (scope) {
  case LOCAL:
    loc_addConst(name);
    fprintf(output, "push %d\n", value);
    return;

  case GLOBAL:
    glo_addConst(name);
    fprintf(output, "%s dq %d\n", name, value);
    return;
  }
}

Type gen_function_declaration(const char name[], int return_type) {
  fprintf(output, "\n%s:\npush rbp\nmov rbp,rsp\n", name);
  return return_type;
}

void gen_function_end_declaration(const char name[], Type return_type, int nb_param) {
  fun_add(name, return_type, nb_param);
  fprintf(output, "mov rax,-1\nmov rsp, rbp\npop rbp\nret\n");
}

void gen_function_return(Type expect, Type actual) {
  if (actual != expect) {
    yyerror("Return type mismatch");
    return;
  }
  if (actual != VOID_T)
    fprintf(output, "pop rax\n");
  fprintf(output, "mov rsp, rbp\npop rbp\nret\n");
}

Type gen_function_call(const char name[], int nb_param) {
  Type return_type = fun_lookup(name, nb_param);
  fprintf(output, "call %s\n", name);
  if (return_type != VOID_T)
    fprintf(output, "push rax\n");
  return return_type;
}

void gen_function_param_passing(int nb_params) {
  for(int i = 1; i <= nb_params; i++) {
    fprintf(output, "mov r8, [rbp + %d]\n", (nb_params - i + 2) * 8);
    fprintf(output, "mov [rbp - %d], r8\n", i * 8);
  }
}

void gen_declaration(const char name[], Type type, Scope scope) {
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

void gen_tab_declaration(const char name[], Scope scope, int size) {
  switch (scope) {
  case GLOBAL:
    glo_addVar(name, TAB);
    nb_globals = nb_globals + size;
    break;
  case LOCAL:
    loc_addVar(name, TAB);
    for (int i = 0; i < size; ++i) {
      fprintf(output, "push 0\n");
    }
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
void gen_reade(const char name[], Scope scope) {
  if (is_read_only(name, scope)) {
    yyerror("Symbol is read only");
    return;
  }

  if (loc_lookup(name) != INT) {
    yyerror("Need to be a INT in the reade() function");
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

void gen_readc(const char name[], Scope scope) {
  if (is_read_only(name, scope)) {
    yyerror("Symbol is read only");
    return;
  }

  if (loc_lookup(name) != CHAR) {
    yyerror("Need to be a CHAR in the readc() function");
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

void gen_print(Type type) {
  // check if the name exists in both tables
  fprintf(output, "pop rax\n");
  switch (type) {
  case INT:
    fprintf(output, "call printe\n");
    break;
  case CHAR:
    fprintf(output, "call printc\n");
    break;
  case TAB:
    fprintf(output, "call printe\n");
    break;
  default:
    yyerror("Error print, supposed to have type CHAR or INT or TAB");
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
  fprintf(output, ";ENDIF\n\n");
}

// ----- OPERATORS -----

static int gen_assign_simple(const char ident[], Scope scope) {
  int l_addr = loc_get_addr(ident);
  int g_addr = glo_get_addr(ident);

  if (is_read_only(ident, scope)) {
    yyerror("Symbol is read only");
    return loc_lookup(ident);
  }

  switch (scope) {
  case GLOBAL:
    fprintf(output, "pop QWORD [globals + %d] ;%s\n", g_addr, ident);
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

static int gen_assign_tab(const char ident[], Scope scope) {
  int l_addr = loc_get_addr(ident);
  int g_addr = glo_get_addr(ident);

  if (is_read_only(ident, scope)) {
    yyerror("Symbol is read only");
    return loc_lookup(ident);
  }

  switch (scope) {
  case GLOBAL:
    fprintf(output, "pop r8 ;EXP\npop rcx ;offset\npush r8\nimul rcx,8\nmov "
                    "rax,globals\n add rax,rcx\npop QWORD [rax + %d] ;%s\n",
            g_addr, ident);
    return glo_lookup(ident);
  case LOCAL:
    if (l_addr != -1) {
      fprintf(output, "pop r8 ;EXP\npop rcx ;offset\npush r8\nimul rcx,8\nmov "
                      "rax,rbp\nsub rax,rcx\npop QWORD [rax - %d] ;%s\n",
              l_addr, ident);
      return loc_lookup(ident);
    } else {
      fprintf(output, "pop r8 ;EXP\npop rcx ;offset\npush r8\nimul rcx,8\nmov "
                      "rax,globals\n add rax,rcx\npop QWORD [rax + %d] ;%s\n",
              g_addr, ident);
      return glo_lookup(ident);
    }
  default:
    exit(1);
  }
}

int gen_assign(const char ident[], Scope scope) {
  switch (loc_lookup(ident)) {
  case TAB: return gen_assign_tab(ident, scope);
  default: return gen_assign_simple(ident, scope);
  }
}

void gen_or(int left, int right, int idx) {
  check_expected_types(left, INT, TAB);
  check_expected_types(right, INT, TAB);

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
  fprintf(output, ".false%d:\n", idx);
}

void gen_and(int left, int right, int idx) {
  check_expected_types(left, INT, TAB);
  check_expected_types(right, INT, TAB);

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
  fprintf(output, ".true%d:\n", idx);
}

void gen_eq(const char op[], int left, int right, int idx) {
  check_expected_types(left, INT, TAB);
  check_expected_types(right, INT, TAB);

  fprintf(output, ";a EQ c\npop rax\npop rcx\ncmp rax,rcx\n");

  if (!strcmp(op, "=="))
    fprintf(output, "je .true%d\n", idx);
  else if (!strcmp(op, "!="))
    fprintf(output, "jne .true%d\n", idx);
  else
    exit(1); // TODO: error on unexpected op

  fprintf(output, "push 0\njmp .false%d\n.true%d:\npush 1\n.false%d:\n", idx,
          idx, idx);
}

void gen_order(const char op[], int left, int right, int idx) {
  check_expected_types(left, INT, TAB);
  check_expected_types(right, INT, TAB);

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

  fprintf(output, "push 0\njmp .false%d\n.true%d:\npush 1\n.false%d:\n", idx,
          idx, idx);
}

void gen_addsub(char op, int left, int right) {
  check_expected_types(left, INT, TAB);
  check_expected_types(right, INT, TAB);

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
  check_expected_types(left, INT, TAB);
  check_expected_types(right, INT, TAB);

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

int gen_signed_expr(char op, Type type) {
  check_expected_types(type, INT, TAB);
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

int gen_negate_expr(Type type) {
  check_expected_types(type, INT, TAB);
  fprintf(output, ";!F\npop rax\nxor rax,1\npush rax\n");
  return type;
}

static int gen_value_simple(const char ident[], Scope scope) {
  int l_addr = loc_get_addr(ident);

  switch (scope) {
  case LOCAL:
    if (l_addr != -1) {
      fprintf(output, "push QWORD [rbp - %d] ;%s\n", l_addr, ident);
      return loc_lookup(ident);
    }

  case GLOBAL:
    if (is_read_only(ident, scope))
      fprintf(output, "push QWORD [%s]\n", ident);
    else
      fprintf(output, "push QWORD [globals + %d] ;%s\n", glo_get_addr(ident),
              ident);

    return glo_lookup(ident);

  default:
    exit(1);
  }
}

static int gen_value_tab(const char ident[], Scope scope) {
  int l_addr = loc_get_addr(ident);
  int g_addr = glo_get_addr(ident);
  switch (scope) {
  case GLOBAL:
    fprintf(output, "pop rcx ;offset\nimul rcx,8\nmov rax,globals\n add "
                    "rax,rcx\npush QWORD [rax + %d] ;%s\n",
            g_addr, ident);
    return glo_lookup(ident);
  case LOCAL:
    if (l_addr != -1) {
      fprintf(output, "pop rcx ;offset\nimul rcx,8\nmov rax,rbp\nsub "
                      "rax,rcx\npush QWORD [rax - %d] ;%s\n",
              l_addr, ident);
      return loc_lookup(ident);
    } else {
      fprintf(output, "pop rcx ;offset\nimul rcx,8\nmov rax,globals\n add "
                      "rax,rcx\npush QWORD [rax + %d] ;%s\n",
              g_addr, ident);
      return glo_lookup(ident);
    }
  default:
    exit(1);
  }
}

int gen_value(const char ident[], Scope scope) {
  switch (loc_lookup(ident)) {
  case TAB: return gen_value_tab(ident, scope);
  default: return gen_value_simple(ident, scope);
  }
}

int gen_num(int value, Scope scope) {
  fprintf(output, "push %d\n", value);
  return INT;
}

int gen_char(int value, Scope scope) {
  fprintf(output, "push %d\n", value);
  return CHAR;
}

void gen_while_start(int id) {
  fprintf(output,".upwhile%d:\n", id);
}

void gen_while_cond(int id) {
  fprintf(output,"pop rax\ncmp rax,0\njz .downwhile%d\n", id);
}

void gen_while_end(int id) {
  fprintf(output,"jmp .upwhile%d\n.downwhile%d:\n", id, id); 
}
