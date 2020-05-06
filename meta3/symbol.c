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
    symbol_t* possible = NULL;

    bool partial;
    bool correct;
    int found = 0;

    while (s != NULL){
        if (strcmp(s->name, name) == 0){
            correct = true;
            partial = false;

            /*Set our formal parameter pointers, one for the symbol's we are trying to find 
                and one for the symbol's we found in the symbol table*/
            paramtypes_t* temp_params = s->paramtypes;
            paramtypes_t* passed_params = params;

            /*As long as both parameter pointers point to existing parameters*/
            while(temp_params && passed_params){
                /*Check if the arguments are an exact match*/
                if (strcmp(temp_params->type, passed_params->type) != 0){
                    /*They are not, but are they compatible? Only an int 
                        for a formal double*/
                    if (strcmp(temp_params->type, "double") == 0
                        && strcmp(passed_params->type, "int") == 0)
                        {
                            /*They are compatible, set the partial flag to true*/
                            partial = true;
                        }

                    /*They are not compatible, wrong method*/
                    else{
                        correct = false;
                        break;
                    }
                }

                temp_params = temp_params->next;
                passed_params = passed_params->next;
            }

            /*If the number of parameters vs arguments differ, wrong method*/
            if (temp_params || passed_params) correct = false;

            /*If we ever hit the partial = true and didn't hit correct = false, means 
                we do not have an exact match, however they are compatible*/
            if (correct && partial){
                /*printf("POSSIBLE: FUNCTION PARAMS (");
                print_params_str(s->paramtypes, false, NULL);
                printf(") FOR CALLED PARAMS(");
                print_params_str(params, false, NULL);
                printf(")\n");*/
                possible = s;
                found += 1;
            }

            /*We do not have a partial match, but do we have an exact one?*/
            else if (correct) return s;

            /*We do not have any match, reset the partial flag*/
            else if (!correct) partial = false;
        }
        s = s->next;
    }
    /*If we've reached this point, means we haven't found an exact match, but 
        may have compatible ones*/

    /*This means we are inserting a new symbol, if we didn't find an exact
        match, means symbol does not exist yet*/
    if (!call){
        return NULL;
    }

    /*Did we find compatible matches?*/
    else if (found > 0){

        /*If we found more than one compatible match, print an 'ambiguous' error*/
        if (found > 1){
            printf("Line %d, col %d: Reference to method %s is ambiguous\n",
                    call->son->line, 
                    (int)(call->son->col - (sizeof(call->name)/sizeof(char*))),
                    name);
            return NULL;
        }

        /*Otherwise return the only possible partial match*/
        else return possible;
    }

    /*We did not find any match whatsoever*/
    if (call){
        printf("Line %d, col %d: Cannot find symbol %s(",
            call->son->line,
            (int)(call->son->col - strlen(name)),
            name);
        print_params_str(params, false, NULL);
        printf(")\n");
    }
    return NULL;
}
