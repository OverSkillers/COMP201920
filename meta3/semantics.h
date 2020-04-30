#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "symbol.h"
#include "ast.h"

extern struct st* global;
extern struct st* cur_table;
extern char aux[1000];

table_t* sem(node* tree,node* begin);

#endif
