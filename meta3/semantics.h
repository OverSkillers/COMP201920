#include "symbol.h"

extern struct st* global;
extern struct st* cur_table;
extern char aux[1000];

void sem(node* tree);
void check_func_parameters(table_t* table, node* cur_node);
void check_func_body(table_t* table, node* cur_node);
bool starts_with(char* start, char* str);
void check_return_statement(node* cur_node, char* func_return, char* expr_return);
void check_if_for_statement(node* cur_node, char* s, char* expr_type);
void check_assign_statement(node* cur_node, char* expr1_type, char* expr2_type);
void check_call_statement(symbol_t* func, node* cur_node);
void check_parseargs_statement(node* cur_node, node* expr1, node* expr2);
void check_binary_expr(node* cur_node, char* s, char* expr1_type, char* expr2_type);
void check_plus_minus_expr(node* cur_node, char* s, char* expr_type);
void check_not_expr(node* cur_node, char* expr_type);
void check_logical_expr(node* cur_node, char* s, char* expr1_type, char* expr2_type);
void check_comp_expr(node* cur_node, char* s, char* expr1_type, char* expr2_type);
void check_statement(table_t* table, node* cur_node);
bool check_int(node* n);
void check_type(table_t* table, node* cur_node);
void check_unused_symbols(table_t* table);
symbol_t* new_vardecl(node* vardecl, bool param);
symbol_t* new_func_global_symbol(node* funcheader);
