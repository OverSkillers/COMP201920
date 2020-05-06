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
        print_params_str(table->paramtypes, false, NULL);
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

void print_params_str(paramtypes_t* params, bool to_string, char* str){
    while (params){
        if (to_string) sprintf(str + strlen(str), "%s", params->type);
        else printf("%s", params->type);
        params = params->next;
        if (params){
            if (to_string) sprintf(str + strlen(str), ",");
            else printf(",");
        }
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
    else if (strcmp(str, "bool") == 0)
        strcpy(symbol->type, "boolean");
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
        symbol_t* found;
        if (symbol->func) found = find_method(global, symbol->name, symbol->paramtypes, NULL);
        else found = find_symbol(table, symbol->name, NULL);

        if (found == NULL){
            table->last->next = symbol;
            table->last = symbol;
        } else {
            printf("Line %d, col %d: Symbol %s already defined\n", 
                symbol->line, (int)(symbol->col - strlen(symbol->name)), symbol->name);
        }
    }
}

symbol_t* find_symbol(table_t* table, char* name, node* src){
    symbol_t* s = table->first;
    while (s != NULL){
        if (strcmp(s->name, name) == 0){
            return s;
        }
        s = s->next;
    }
    if (strcmp(table->type, GLOBAL_TABLE_TYPE) == 0){
        if (src){
            printf("Line %d, col %d: Cannot find symbol %s\n", 
                    src->line, (int)(src->col - (sizeof(src->name)/sizeof(char*))), name);
        }
        return NULL;
    }
    else return find_symbol(global, name, src);
}

symbol_t* find_method(table_t* table, char* name, paramtypes_t* params, node* call){
    symbol_t* s = table->first;
    // TODO: Print errors
    while (s != NULL){
        if (strcmp(s->name, name) == 0){
            /*Check the arguments*/
            bool correct = true;
            paramtypes_t* temp_params = s->paramtypes;
            paramtypes_t* passed_params = params;
            while(temp_params && passed_params){
                // TODO: Compatible arguments (int where double is required)
                // TODO: Maybe boolean with int as well?
                if (strcmp(temp_params->type, passed_params->type) != 0){
                    correct = false;
                    break;
                }

                temp_params = temp_params->next;
                passed_params = passed_params->next;
            }

            if (temp_params || passed_params) correct = false;

            if (correct) return s;
        }
        s = s->next;
    }
    if (call){
        printf("Line %d, col %d: Cannot find symbol %s (",
            call->son->line, (int)(call->son->col - (sizeof(call->name)/sizeof(char*))), name);
        print_params_str(params, false, NULL);
        printf(")\n");
    }
    return NULL;
}
