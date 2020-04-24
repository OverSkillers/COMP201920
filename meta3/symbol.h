#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

struct st;
struct sym;
struct pt;

struct st* global;
char aux[1000];

typedef struct st {
    char* name;
    struct sym* first;
    struct sym* last;
    struct st* next;
} table_t;

typedef struct sym {
    char* name;
    char* type;
    struct pt* paramtypes;
    int line, col;
    bool param;
    bool func;
    bool used;
    struct sym* next;
} symbol_t;

typedef struct pt {
    char* type;
    struct pt* next;
} paramtypes_t;

void get_id(const char* id);
void print_table(table_t* table);
table_t* new_table(const char* name);
table_t* new_function_table(const char* name, char* return_type, paramtypes_t* pt);
symbol_t* new_symbol(const char* name, const char* type, paramtypes_t* paramtypes, bool param, bool func, int line, int col);
symbol_t* new_var_symbol(const char* name, const char* type, bool param, int line, int col);
symbol_t* new_function_symbol(node* funcheader);
paramtypes_t* new_parameter(const char* type);
paramtypes_t* new_paramtypes(node* paramdecl);
void insert_symbol(table_t* table, symbol_t* symbol);
symbol_t* find_symbol(table_t* table, char* name);
