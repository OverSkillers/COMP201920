#include <ctype.h>
#include "semantics.h"

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
        else if (strcmp(str, "bool") == 0)
            new_params->type = strdup("boolean");
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
        else if(strcmp(CALL, temp->name) == 0){
            check_call(global_table, method_table, temp);
        }
        else if (is_expr(temp)){
            check_expression(global_table, method_table, temp);
        }
        else if (is_assignment(temp)){
            check_assignment(global_table, method_table, temp);
        }

        temp = temp->next;
    }
}

void check_if_stmt(table_t* global_table, table_t* method_table, node* if_node){
    if(if_node->annotation != NULL){
        if(strcmp(if_node->annotation,"boolean")!=0){
            printf("Line %d, col %d: Incompatible type %s in if  statement\n", if_node->line, if_node->col, if_node->annotation);
        }
    }
}

void check_call(table_t* global_table, table_t* method_table, node* call_node){
    /*Create param types*/
    paramtypes_t* params = NULL;
    paramtypes_t* current = params;

    /*Check arguments*/
    node* arguments = call_node->son->next;
    while(arguments){
        if (is_expr(arguments)) check_expression(global_table, method_table, arguments);
        else if (is_assignment(arguments)) check_assignment(global_table, method_table, arguments);
        else if (strcmp(arguments->name, "Id") == 0){
            symbol_t* symbol = find_symbol(method_table, arguments->type);
            if (symbol == NULL) arguments->annotation = strdup("undef");
            else arguments->annotation = strdup(symbol->type);
        }

        // TODO: Free this memory
        paramtypes_t* new_params = malloc(sizeof(struct pt));
        if (params == NULL) params = new_params;
        else current->next = new_params;

        new_params->type = strdup(arguments->annotation);
        new_params->next = NULL;

        current = new_params;
        arguments = arguments->next;
    }
    /*Check if arguments match with method signature*/
    node* id = call_node->son;
    symbol_t* method = find_method(global, id->type, params);

    char *str = malloc(sizeof(MAX_PARAMS_ANNOTATION_LEN));
    set_annotation(call_node->son, "undef");

    /*Annotate call with return type and id node with paramtypes*/
    if (method == NULL){
        // TODO: Print error
        set_annotation(call_node, "undef");
    }
    else{
        /*Annotate call id node with paramtypes*/
        strcpy(str, "(");
        print_params_str(method->paramtypes, true, str);
        strcat(str, ")");

        set_annotation(call_node->son, str);
        set_annotation(call_node, method->type);
    }

    /*TODO: Free params*/
    //free_paramtypes_t(params);
}

void check_expression(table_t* global_table, table_t* method_table, node* expr){
    /*If left child has no annotation, check it as well*/
    node* left = expr->son;
    if (!left->annotation){
        if (is_expr(left)) check_expression(global_table, method_table, left);
        else if (is_assignment(left)) check_assignment(global_table, method_table, left);
        else if (strcmp(left->name, "Id") == 0){
            symbol_t* symbol = find_symbol(method_table, left->type);
            if (!symbol) left->annotation = strdup("undef");
            else left->annotation = strdup(symbol->type);
        }
    }

    /*If right child exists and has no annotation, check it as well*/
    node* right = expr->son->next;
    if (right && !right->annotation){
        if (is_expr(right)) check_expression(global_table, method_table, right);
        else if (is_assignment(right)) check_assignment(global_table, method_table, right);
    }

    /*If only one child, check for valid expression (plus, minus and not)*/
    if (!right){
        if (strcmp(expr->name, "Plus") == 0
            || strcmp(expr->name, "Minus") == 0)
            {
                /*Rule out string, String[], void and boolean*/
                if (strcmp(left->annotation, "int") != 0
                    && strcmp(left->annotation, "double") != 0)
                    {
                        // TODO: Print error
                        expr->annotation = strdup("undef");
                    }
                else expr->annotation = strdup(left->annotation);
            }

        else if (strcmp(expr->name, "Not") == 0){
            /*Only boolean type is accepted*/
            if (strcmp(left->annotation, "boolean") != 0){
                // TODO: Print error
                        expr->annotation = strdup("undef");
            }
            else expr->annotation = strdup(left->annotation);
        }
    }
    else{
        /*Check if expression is valid (and annotate it)*/
        annotate_expression(left, right, expr);
    }
}

void annotate_expression(node* left, node* right, node* expr){
    // FIXME: Maybe refactor this mess
    if (strcmp(expr->name, "Add") == 0
        || strcmp(expr->name, "Sub") == 0
        || strcmp(expr->name, "Div") == 0
        || strcmp(expr->name, "Mul") == 0
        || strcmp(expr->name, "Mod") == 0)
        {
            /*Rule out string, String[], void and boolean*/
            if ((strcmp(left->annotation, "int") != 0
                && strcmp(left->annotation, "double") != 0)
                || (strcmp(right->annotation, "int") != 0
                && strcmp(right->annotation, "double") != 0))
                {
                    // TODO: Print error
                    expr->annotation = strdup("undef");
                }

            else{
                /*If either type is double, expression type will be double*/
                if (strcmp(left->annotation, "double") == 0
                    || strcmp(right->annotation, "double") == 0)
                    {
                        expr->annotation = strdup("double");
                    }

                /*Only int type remains*/
                else expr->annotation = strdup("int");
            }
        }

    else if(strcmp(expr->name, "Xor") == 0
           || strcmp(expr->name, "And") == 0
           || strcmp(expr->name, "Or") == 0
           || strcmp(expr->name, "Not") == 0
           || strcmp(expr->name, "Lt") == 0
           || strcmp(expr->name, "Gt") == 0
           || strcmp(expr->name, "Le") == 0
           || strcmp(expr->name, "Ge") == 0
           || strcmp(expr->name, "Ne") == 0
           || strcmp(expr->name, "Eq") == 0)
           {
                /*Only boolean is accepted*/
                if (strcmp(left->annotation, "boolean") != 0
                    || strcmp(right->annotation, "boolean") != 0)
                    {
                        // TODO: Print error
                        expr->annotation = strdup("undef");
                    }

                else expr->annotation = strdup("boolean");
           }

    else if(strcmp(expr->name, "Lshift") == 0
            || strcmp(expr->name, "Rshift") == 0)
            {
                /*TODO: Only int is accepted?*/
                if (strcmp(left->annotation, "int") == 0
                    || strcmp(right->annotation, "int") == 0)
                    {
                        // TODO: Print error
                        expr->annotation = strdup("undef");
                    }

                else expr->annotation = strdup("int");
            }
}

void check_assignment(table_t* global_table, table_t* method_table, node* assign){

}

bool is_expr(node* src){
    return strcmp(src->name, "Sub") == 0
           || strcmp(src->name, "Mul") == 0
           || strcmp(src->name, "Div") == 0
           || strcmp(src->name, "Mod") == 0
           || strcmp(src->name, "Add") == 0
           || strcmp(src->name, "Xor") == 0
           || strcmp(src->name, "And") == 0
           || strcmp(src->name, "Or") == 0
           || strcmp(src->name, "Not") == 0
           || strcmp(src->name, "Lt") == 0
           || strcmp(src->name, "Gt") == 0
           || strcmp(src->name, "Le") == 0
           || strcmp(src->name, "Ge") == 0
           || strcmp(src->name, "Ne") == 0
           || strcmp(src->name, "Eq") == 0
           || strcmp(src->name, "Lshift") == 0
           || strcmp(src->name, "Rshift") == 0
           || strcmp(src->name, "Minus") == 0
           || strcmp(src->name, "Plus") == 0;
}

bool is_assignment(node* src){
    return strcmp(src->name, "Assign") == 0;
}
