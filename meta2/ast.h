#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "structures.h"

/*node* create_node(const char* name);
void add_next(node* n, node* n2);
void add_son(node* n, node* son);
bool is_block(node* n);
node* new_id(const char* name);
void print_tree(int level, node* n);
void free_tree(node* n);*/


program* insert_program(char*, field_decl_list*, method_decl_list*);
method_decl_list* insert_method_decl(method_decl_list*, method_decl*);
field_decl* create_field_decl(t_var, char*);
field_decl_list* insert_field_decl(field_decl_list*, t_var, field_decl_ids*);
field_decl_ids* insert_field_decl_id(field_decl_ids*, char*);
method_decl* create_method_decl(method_header*, method_body*);
method_header* create_method_header(t_var, char*, param_decl_list*);
param_decl_list* insert_param_decl(param_decl_list*, param_decl*);
param_decl* create_param_decl(t_var, char*);
method_body* create_method_body(method_body_nodes*);
method_body_nodes* insert_mbody_statement(method_body_nodes*, statement_list*);
method_body_nodes* insert_mbody_vardecl(method_body_nodes*, vardecl_list*);
vardecl* create_vardecl(t_var, char*);
vardecl_ids* insert_vardecl_id(vardecl_ids*, char*);
vardecl_list* insert_vardecl(vardecl_list*, t_var, vardecl_ids*);
expression* insert_expression_node(t_expr, char*, char*);

void print_tree(program*);