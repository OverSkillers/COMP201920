#include "symbol.h"

void get_id(const char* id){
    memset(aux, 0, 1000);
    strcpy(aux, id + 3);
    aux[strlen(aux)-1] = '\0';
}

void print_table(table_t* table){
    if (table == NULL) return;
    printf("===== %s Symbol Table =====\n", table->name);
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

table_t* new_table(const char* name){
    table_t* table = (table_t *) malloc(sizeof(table_t));
    table->name = strdup(name);
    table->first = NULL;
    table->last = NULL;
    table->next = NULL;
    return table;
}

table_t* new_function_table(const char* name, char* return_type, paramtypes_t* pt){
    char arr[1000];
    strcpy(arr, "Method ");
    get_id(name);
    strcat(arr, aux);
    strcat(arr, "(");
    if (pt != NULL){
        strcat(arr, pt->type);
        pt = pt->next;
        while (pt != NULL){
            strcat(arr, ","); strcat(arr, pt->type);
            pt = pt->next;
        }
    }
    strcat(arr, ")");
    table_t* functable = new_table(arr);
    if (return_type == NULL) insert_symbol(functable, new_symbol("return", "none", NULL, false, false, 0, 0));
    else insert_symbol(functable, new_symbol("return", return_type, NULL, false, false, 0, 0));
    return functable;
}

symbol_t* new_symbol(const char* name, const char* type, paramtypes_t* paramtypes, bool param, bool func, int line, int col){
    symbol_t* symbol = (symbol_t *) malloc(sizeof(symbol_t));
    symbol->name = strdup(name);
    strcpy(symbol->type, type);
    symbol->param = param;
    symbol->paramtypes = paramtypes;
    symbol->func = func;
    // Se é funcao é "usado" por default, se for variavel nao é usado automaticamente
    symbol->used = func;
    symbol->line = line;
    symbol->col = col;
    symbol->next = NULL;
    return symbol;
}

symbol_t* new_var_symbol(const char* name, const char* type, bool param, int line, int col){
    return new_symbol(name, type, NULL, param, false, line, col);
}

symbol_t* new_function_symbol(node* funcheader){
    node* id = funcheader->son;
    get_id(id->name);
    //TODO: Change this size
    char* type = malloc(1000);
    strcpy(type, id->next->name);
    if (strcmp(id->next->name, "MethodParams") == 0){ // nó type nao esta no funcheader, logo no[1] = params
        return new_symbol(aux, NULL, new_paramtypes(id->next->son), false, true, funcheader->line, funcheader->col);
    } else { // no[2] = params
        return new_symbol(aux, id->next->name, new_paramtypes(id->next->next->son), false, true, funcheader->line, funcheader->col);
    }
}

paramtypes_t* new_parameter(const char* type){
    paramtypes_t* param = (paramtypes_t *) malloc(sizeof(paramtypes_t));
    strcpy(param->type, type);
    param->next = NULL;
    return param;
}

paramtypes_t* new_paramtypes(node* paramdecl){
    if (paramdecl == NULL) return NULL;
    paramtypes_t* newparam = (paramtypes_t *) malloc(sizeof(paramtypes_t));
    newparam->type = paramdecl->son->name;
    newparam->next = new_paramtypes(paramdecl->next);
    return newparam;
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
