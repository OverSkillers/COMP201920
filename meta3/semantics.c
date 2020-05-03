#include <ctype.h>
#include "semantics.h"

#define FUNC_DECL "MethodDecl"
#define FIELD_DECL "FieldDecl"
#define VAR_DECL "VarDecl"
#define IF_STMT "If"

table_t* sem(node* tree, node* begin){
    global = new_table(tree->son->type, GLOBAL_TABLE_TYPE, NULL);
    // temp = vardecl ou funcdecl
    node* temp = tree->son; 
    table_t* global_table = global;
    table_t* last_table = global_table;
    while(temp){
        // FIXME ? Is it temp->name ?
        if (strcmp(FUNC_DECL, temp->name) == 0){
            last_table = check_func_decl(global_table, last_table, temp);
        }
        else if (strcmp(FIELD_DECL, temp->name) == 0){
            check_field_decl(global_table, temp);
        }

        temp = temp->next;
    }

    return global;
}


table_t* check_func_decl(table_t* global_table, table_t* last_table, node* func_decl){
    // TODO: Check if method already exists
    /*Create param types*/
    paramtypes_t* params = NULL;
    paramtypes_t* current = params;
    node* method_params = func_decl->son->son->next->next;
    node* param_decl = method_params->son;

    /*Create new symbol table for new method*/
    table_t* new_symbol_table = new_table(func_decl->son->son->next->type,
                                        METHOD_TABLE_TYPE,
                                        func_decl->son->son->name);
    last_table->next = new_symbol_table;

    while(param_decl){
        // TODO: Free this memory
        paramtypes_t* new_params = malloc(sizeof(struct pt));
        if (params == NULL) params = new_params;
        else current->next = new_params;

        /*Convert the type to lower case*/
        char* str = strdup(param_decl->son->name);
        *str = tolower(*str);

        // TODO: Free this memory
        if (strcmp(str, "stringArray") == 0)
            new_params->type = strdup("String[]");
        else new_params->type = strdup(str);
        new_params->next = NULL;
        free(str);

        /*Insert param symbols into new table*/
        insert_symbol(new_symbol_table, create_symbol(param_decl, false, true, NULL));

        current = new_params;
        param_decl = param_decl->next;
    }
    new_symbol_table->paramtypes = params;

    /*Insert param types into global symbol table*/
    insert_symbol(global_table, create_symbol(func_decl->son, true, false, params));

    node* method_body = func_decl->son->next;
    check_method_body(global_table, new_symbol_table, method_body);

    return new_symbol_table;
}

void check_field_decl(table_t* global_table, node* field_decl){
    /**/
    if (field_decl->son && field_decl->son->next){
        insert_symbol(global_table, create_symbol(field_decl, false, false, NULL));
    }
}

void check_method_body(table_t* global_table, table_t* method_table, node* method_body){
    node* temp = method_body->son;

    while(temp){
        if (strcmp(VAR_DECL, temp->name) == 0){
            insert_symbol(method_table, create_symbol(temp, false, false, NULL));
        }

        else if (strcmp(IF_STMT, temp->name) == 0){
            check_if_stmt(global_table, method_table, temp);
        }

        temp = temp->next;
    }
}

void check_if_stmt(table_t* global_table, table_t* method_table, node* if_node){

}