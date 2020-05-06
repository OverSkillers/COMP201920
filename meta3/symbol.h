#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

#define GLOBAL_TABLE_TYPE "Class"
#define METHOD_TABLE_TYPE "Method"
#define MAX_SYMBOL_NAME_LEN 200
#define MAX_SYMBOL_TYPE_LEN 200
#define MAX_PARAM_TYPE_LEN 300

struct st;
struct sym;
struct pt;

struct st* global;
char aux[1000];

typedef struct st {
    char* name;
    char* type;
    char* return_type;
    struct pt* paramtypes;
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

void get_id(const char*);
void print_table(table_t*);
void print_params_str(paramtypes_t*, bool, char*);
table_t* new_table(const char*, const char*, const char*);
symbol_t* create_symbol(node*, bool, bool, paramtypes_t*);
void insert_symbol(table_t*, symbol_t*);
symbol_t* find_symbol(table_t*, char*, node*);
symbol_t* find_method(table_t*, char*, paramtypes_t*, node*);

#endif
