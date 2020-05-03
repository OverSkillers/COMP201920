#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "symbol.h"
#include "ast.h"

extern struct st* global;
extern struct st* cur_table;
extern char aux[1000];

table_t* sem(node*, node*);
table_t* check_func_decl(table_t*, table_t*, node*);
void check_field_decl(table_t*, node*);
void check_method_body(table_t*, table_t*, node*);
void check_if_stmt(table_t*, table_t*, node*);

#endif
