#include "symbol.h"

void get_id(const char* id){
    memset(aux, 0, 1000);
    strcpy(aux, id + 3);
    aux[strlen(aux)-1] = '\0';
}

void print_table(table_t* table){
    if (table == NULL) return;
    printf("===== %s %s Symbol Table =====\n", table->name,table->begin->type);
    symbol_t* s = table->first;
    while (s != NULL){
        printf("%s\t", s->name);
        if (strcmp(table->name, "Class") == 0){
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

table_t* new_table(const char* name,node* begin){
    table_t* table = (table_t *) malloc(sizeof(table_t));
    table->name = strdup(name);
    table->first = NULL;
    table->last = NULL;
    table->next = NULL;
    table->begin = begin;
    return table;
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
            printf("Line %d, column %d: Symbol %s already defined\n", symbol->line, symbol->col, symbol->name);
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
    if (strcmp(table->name, "Global") == 0) return NULL;
    else return find_symbol(global, name);
}
