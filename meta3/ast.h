#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct _args{
    int line;
    int col;
    char *val;
};

typedef struct n{
	char *name;
	char *type;
    char *annotation;
    char* literal;
	struct n* son;
	struct n* next;
    int line;
    int col;
    int param;
    struct n* prev;
} node;

node* create_node(const char* name,const char* type, int line, int col);
void set_annotation(node*, const char*);
void set_literal_symbol(node*, const char*);
void add_next(node* n, node* n2);
void add_son(node* n, node* son);
bool is_block(node* n);
void print_tree(int level, node* n, int abs);
void free_tree(node* n);
int getLine(struct _args* n);
int getCol(struct _args* n);
int getLineN(node* n);
int getColN(node* n);

#endif