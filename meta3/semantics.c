#include <ctype.h>
#include "semantics.h"

#define FUNC_DECL "MethodDecl"
#define FIELD_DECL "FieldDecl"

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


table_t* check_func_decl(table_t* current_table, table_t* last_table, node* func_decl){
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
        char* str = param_decl->son->name;
        for ( ; *str; ++str) *str = tolower(*str);

        // TODO: Free this memory
        if (strcmp(param_decl->son->name, "stringarray") == 0)
            new_params->type = strdup("String[]");
        else new_params->type = strdup(param_decl->son->name);
        new_params->next = NULL;

        /*Insert param symbols into new table*/
        insert_symbol(new_symbol_table, create_symbol(param_decl, false, true, NULL));

        current = new_params;
        param_decl = param_decl->next;
    }
    new_symbol_table->paramtypes = params;

    /*Insert param types into global symbol table*/
    insert_symbol(current_table, create_symbol(func_decl->son, true, false, params));

    node* method_body = func_decl->son->next;
    check_method_body(current_table, method_body);

    return new_symbol_table;
}

void check_field_decl(table_t* global, node* field_decl){
    /**/
    if (field_decl->son && field_decl->son->next){
        insert_symbol(global, create_symbol(field_decl, false, false, NULL));
    }
}

void check_method_body(table_t* table, node* method_body){

}