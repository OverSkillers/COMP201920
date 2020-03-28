#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "structures.h"

node* create_node(const char* name);
void add_next(node* n, node* n2);
void add_son(node* n, node* son);
bool is_block(node* n);
node* new_id(const char* name);
void print_tree(int level, node* n);
void free_tree(node* n);
