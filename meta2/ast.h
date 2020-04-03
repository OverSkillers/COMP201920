#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct n{
	char *name;
	char *type;
	struct n* son;
	struct n* next;
} node;

node* create_node(const char* name,const char* type);
node* change_son_node(node *origin, node *new_son);
void add_next(node* n, node* n2);
void add_son(node* n, node* son);
bool is_block(node* n);
node* new_id(const char* name,const char* type);
void print_tree(int level, node* n);
void free_tree(node* n);
