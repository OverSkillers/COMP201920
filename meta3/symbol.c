#include <ctype.h>
#include "symbol.h"

void get_id(const char* id){
    memset(aux, 0, 1000);
    strcpy(aux, id + 3);
    aux[strlen(aux)-1] = '\0';
}

void print_table(table_t* table){
    if (table == NULL) return;
    /*Check if parameter type printing is needed*/
    if (strcmp(table->type, GLOBAL_TABLE_TYPE) != 0){
        printf("===== %s %s(", table->type, table->name);
        print_params_str(table);
        printf(") Symbol Table =====\n");
    }
    else printf("===== %s %s Symbol Table =====\n", table->type, table->name);

    /*Print return type if it exists*/
    if (table->return_type != NULL){
        /*Convert the type to lower case*/
        char* str = strdup(table->return_type);
        *str = tolower(*str);
        printf("return\t\t%s\n", str);
        free(str);
    }

    symbol_t* s = table->first;
    while (s != NULL){
        printf("%s\t", s->name);
        if (strcmp(table->type, GLOBAL_TABLE_TYPE) == 0){
            if (s->func) printf("(");
            if (s->paramtypes != NULL){
                printf("%s", s->paramtypes->type);
                paramtypes_t* pt = s->paramtypes->next;
                while (pt != NULL){
                    printf(",%s", pt->type);
                    pt = pt->next;
                }
            }
            if (s->func) printf(")");
        }
        printf("\t");
        printf("%s", s->type);
        if (s->param) printf("\tparam");
        printf("\n");
        s = s->next;
    }
    printf("\n");
    print_table(table->next);
}

void print_params_str(table_t* table){
    paramtypes_t* params = table->paramtypes;
    while (params){
        printf("%s", params->type);
        params = params->next;
        if (params) printf(",");
    }
}

table_t* new_table(const char* name, const char* type, const char* return_type){
    table_t* table = (table_t *) malloc(sizeof(table_t));
    // TODO: free this memory
    table->name = strdup(name);
    table->first = NULL;
    table->last = NULL;
    table->next = NULL;
    // TODO: free this memory
    table->type = strdup(type);
    // TODO: free this memory
    if (return_type) table->return_type = strdup(return_type);
    return table;
}

symbol_t* create_symbol(node* src, bool is_func, bool is_param, paramtypes_t* pt){
    symbol_t* symbol = malloc(sizeof(struct sym));
    symbol->next = NULL;
    symbol->col = src->col;
    symbol->line = src->line;
    // TODO: Free this memory
    symbol->name = malloc(MAX_SYMBOL_NAME_LEN);
    strcpy(symbol->name, src->son->next->type);
    symbol->func = is_func;
    symbol->param = is_param;
    // TODO: Free this memory
    symbol->type = malloc(MAX_SYMBOL_TYPE_LEN);

    /*Convert the type to lower case*/
    char* str = strdup(src->son->name);
    *str = tolower(*str);

    if (strcmp(str, "stringArray") == 0)
        strcpy(symbol->type, "String[]");
    else strcpy(symbol->type, str);
    symbol->paramtypes = pt;

    free(str);
    return symbol;
}

void insert_symbol(table_t* table, symbol_t* symbol){
    if (table->first == NULL){
        table->first = symbol;
        table->last = symbol;
    } else {
        if (find_symbol(table, symbol->name) == NULL){
            table->last->next = symbol;
            table->last = symbol;
        } else {
            printf("Line %d, col %d: Symbol %s already defined\n", 
                symbol->line, symbol->col, symbol->name);
        }
    }
}

symbol_t* find_symbol(table_t* table, char* name){
    symbol_t* s = table->first;
    while (s != NULL){
        if (strcmp(s->name, name) == 0){
            return s;
        }
        s = s->next;
    }
    if (strcmp(table->type, GLOBAL_TABLE_TYPE) == 0) return NULL;
    else return find_symbol(global, name);
}
