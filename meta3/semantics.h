#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "symbol.h"
#include "ast.h"

#define MAX_PARAMS_ANNOTATION_LEN 1000
#define FUNC_DECL "MethodDecl"
#define FIELD_DECL "FieldDecl"
#define VAR_DECL "VarDecl"
#define IF_STMT "If"
#define CALL_STMT "Call"
#define RETURN_STMT "Return"
#define WHILE_STMT "While"
#define PARSEARGS_STMT "ParseArgs"
#define PRINT_STMT "Print"

extern struct st* global;
extern struct st* cur_table;
extern char aux[1000];

table_t* sem(node*, node*);
table_t* check_func_decl(table_t*, table_t*, node*);
void check_field_decl(table_t*, node*);
void check_method_body(table_t*, table_t*, node*);
void check_if(table_t*, table_t*, node*);
void check_call(table_t*, table_t*, node*);
void check_expression(table_t*, table_t*, node*);
void annotate_expression(node*, node*, node*);
void check_assignment(table_t*, table_t*, node*);
void check_block(table_t*, table_t*, node*);
void check_statement(table_t*, table_t*, node*);
void check_return(table_t*, table_t*, node*);
void check_while(table_t*, table_t*, node*);
void check_parse_args(table_t*, table_t*, node*);
void check_print(table_t*, table_t*, node*);
bool is_expr(node*);
bool is_assignment(node*);
bool is_block_node(node*);
bool is_statement(node*);

#endif
