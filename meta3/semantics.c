#include <ctype.h>
#include "semantics.h"

table_t* sem(node* tree, node* begin){
    global = new_table(tree->son->type, GLOBAL_TABLE_TYPE, NULL);
    // temp = vardecl ou funcdecl
    node* temp = tree->son; 
    table_t* global_table = global;
    table_t* last_table = global_table;

    /*Save the MethodBody's we find for last*/
    struct methodbody_list{
        node* method_body;
        table_t* method_table;
        struct methodbody_list* next;
    };

    struct methodbody_list* methods = NULL;
    struct methodbody_list* current = methods;

    while(temp){
        // FIXME ? Is it temp->name ?
        if (strcmp(FUNC_DECL, temp->name) == 0){
            table_t* temp_last_table;
            temp_last_table = check_func_decl(global_table, last_table, temp);

            /*Did we receive a valid table? If not, do not check this method body
                because function is invalid*/
            if (temp_last_table != NULL){
                last_table = temp_last_table;

                struct methodbody_list* new_body = malloc(sizeof(struct methodbody_list));
                if (methods == NULL) methods = new_body;
                else current->next = new_body;

                new_body->method_body = temp->son->next;
                new_body->method_table = last_table;
                new_body->next = NULL;

                current = new_body;
            }
        }

        else if (strcmp(FIELD_DECL, temp->name) == 0){
            check_field_decl(global_table, temp);
        }

        temp = temp->next;
    }

    /*After checking every function declaration, check the method bodies*/
    current = methods;
    while(current){
        check_method_body(global_table, current->method_table, current->method_body);

        current = current->next;
    }

    return global;
}


table_t* check_func_decl(table_t* global_table, table_t* last_table, node* func_decl){
    /*Create param types*/
    paramtypes_t* params = NULL;
    paramtypes_t* current = params;
    node* method_params = func_decl->son->son->next->next;
    node* param_decl = method_params->son;

    /*Check funcDecl name*/
    if (strcmp(func_decl->son->son->next->type, "_") == 0){
        printf("Line %d, col %d: Symbol _ is reserved\n",
                func_decl->line, func_decl->col);
        return NULL;
    }

    /*Create new symbol table for new method*/
    char *type_str = strdup(func_decl->son->son->name);
    *type_str = tolower(*type_str);
    if (strcmp(type_str, "stringArray") == 0)
        strcpy(type_str, "String[]");
    else if (strcmp(type_str, "bool") == 0)
        strcpy(type_str, "boolean");

    table_t* new_symbol_table = new_table(func_decl->son->son->next->type,
                                        METHOD_TABLE_TYPE,
                                        type_str);
    free(type_str);

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
        insert_symbol(new_symbol_table, create_symbol(param_decl, false, true, NULL), true);

        current = new_params;
        param_decl = param_decl->next;
    }
    new_symbol_table->paramtypes = params;

    /*Insert new method into global symbol table if not existing yet*/
    if (insert_symbol(global_table, create_symbol(func_decl->son, true, false, params), false)){
        last_table->next = new_symbol_table;
        return new_symbol_table;
    }
    else return NULL;
}

void check_field_decl(table_t* global_table, node* field_decl){
    /**/
    if (field_decl->son && field_decl->son->next){
        /*Check if var name is allowed*/
        if (strcmp(field_decl->son->next->type, "_") != 0)
            insert_symbol(global_table, create_symbol(field_decl, false, false, NULL), false);
        else printf("Line %d, col %d: Symbol _ is reserved\n",
                field_decl->line, field_decl->col - 1);
    }
}

void check_method_body(table_t* global_table, table_t* method_table, node* method_body){
    node* temp = method_body->son;

    while(temp){
        if (strcmp(VAR_DECL, temp->name) == 0){
            /*Check if var name is allowed*/
            if (strcmp(temp->son->next->type, "_") != 0)
                insert_symbol(method_table, create_symbol(temp, false, false, NULL), true);
            else printf("Line %d, col %d: Symbol _ is reserved\n",
                    temp->line, temp->col - 1);
        }
        else if (strcmp(IF_STMT, temp->name) == 0){
            check_if(global_table, method_table, temp);
        }
        else if(strcmp(CALL_STMT, temp->name) == 0){
            check_call(global_table, method_table, temp);
        }
        else if (is_expr(temp)){
            check_expression(global_table, method_table, temp);
        }
        else if (is_assignment(temp)){
            check_assignment(global_table, method_table, temp);
        }
        else if (strcmp(RETURN_STMT, temp->name) == 0){
            check_return(global_table, method_table, temp);
        }
        else if (strcmp(WHILE_STMT, temp->name) == 0){
            check_while(global_table, method_table, temp);
        }
        else if (strcmp(PARSEARGS_STMT, temp->name) == 0){
            check_parse_args(global_table, method_table, temp);
        }
        else if (strcmp(PRINT_STMT, temp->name) == 0){
            check_print(global_table, method_table, temp);
        }

        temp = temp->next;
    }
}

void check_if(table_t* global_table, table_t* method_table, node* if_node){
    /*Check left node*/
    node* left = if_node->son->next;
    /*If left node has no annotation, check it as well*/
    if (!left->annotation){
        if (is_block_node(left)) check_block(global_table, method_table, left);
        else if (is_statement(left)) check_statement(global_table, method_table, left);
    }

    /*Check right node*/
    node* right = if_node->son->next->next;
    /*If right node has no annotation, check it as well*/
    if (!right->annotation){
        if (is_block_node(right)) check_block(global_table, method_table, right);
        else if (is_statement(right)) check_statement(global_table, method_table, right);
    }

    /*Check condition*/
    node* condition = if_node->son;
    /*If condition has no annotation, check it as well*/
    if (!condition->annotation){
        if (is_expr(condition)) 
            check_expression(global_table, method_table, condition);
        else if (is_assignment(condition))
            check_assignment(global_table, method_table, condition);
    }

    /*Semantically check the if statement, only boolean accepted in condition*/
    if (condition->annotation){
        if (strcmp(condition->annotation, "boolean") != 0){
            // FIXME: Wrong col number
            printf("Line %d, col %d: Incompatible type %s in if statement\n", 
                    if_node->line, if_node->col, condition->annotation);
        }
    }
    // FIXME: Wrong col number
    else printf("Line %d, col %d: Incompatible type %s in if statement\n", 
                    if_node->line, if_node->col, "undef");
}

void check_call(table_t* global_table, table_t* method_table, node* call_node){
    /*Create param types*/
    paramtypes_t* params = NULL;
    paramtypes_t* current = params;

    /*Check arguments*/
    node* arguments = call_node->son->next;
    while(arguments){
        if (is_expr(arguments)) 
            check_expression(global_table, method_table, arguments);

        else if (is_assignment(arguments)) 
            check_assignment(global_table, method_table, arguments);

        else if (strcmp(arguments->name, "Call") == 0) 
            check_call(global_table, method_table, arguments);

        // TODO: Free this memory
        paramtypes_t* new_params = malloc(sizeof(struct pt));
        if (params == NULL) params = new_params;
        else current->next = new_params;

        if (arguments->annotation) new_params->type = strdup(arguments->annotation);
        else new_params->type = strdup("none");
        new_params->next = NULL;

        current = new_params;
        arguments = arguments->next;
    }
    /*Check if arguments match with method signature*/
    node* id = call_node->son;
    symbol_t* method = find_method(global, id->type, params, call_node);

    char *str = malloc(sizeof(MAX_PARAMS_ANNOTATION_LEN));
    set_annotation(call_node->son, "undef");

    /*Annotate call with return type and id node with paramtypes*/
    if (method == NULL){
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
    /*If it's an id ...*/
    if (strcmp(expr->name, "Id") == 0){
        /*... and has any children, is a .length, annotate it with int*/
        if (expr->son){
            expr->annotation = strdup("int");
        }

        else if (strcmp(expr->type, "args") == 0){
            expr->annotation = strdup("String[]");
        }

        /*Else just look in the symbol tables*/
        else{
            symbol_t* symbol = find_symbol(method_table, expr->type, expr, false);
            if (symbol == NULL) expr->annotation = strdup("undef");
            else expr->annotation = strdup(symbol->type);
        }

        return;
    }

    // TODO: Check limits
    /*If it's a declit, boollit or reallit*/
    if (strcmp(expr->name, "DecLit") == 0){
        expr->annotation = strdup("int");
        return;
    }
    if (strcmp(expr->name, "RealLit") == 0){
        expr->annotation = strdup("double");
        return;
    }
    if (strcmp(expr->name, "BoolLit") == 0){
        expr->annotation = strdup("boolean");
        return;
    }
    if (strcmp(expr->name, "StrLit") == 0){
        expr->annotation = strdup("string");
        return;
    }


    /*If left child has no annotation, check it as well*/
    node* left = expr->son;
    if (!left->annotation){
        if (is_expr(left)) 
            check_expression(global_table, method_table, left);
        else if (is_assignment(left)) 
            check_assignment(global_table, method_table, left);
        else if (strcmp(left->name, "Id") == 0){
            symbol_t* symbol = find_symbol(method_table, left->type, left, false);
            if (!symbol) left->annotation = strdup("undef");
            else left->annotation = strdup(symbol->type);
        }
    }

    /*If right child exists and has no annotation, check it as well*/
    node* right = expr->son->next;
    if (right && !right->annotation){
        if (is_expr(right)) 
            check_expression(global_table, method_table, right);
        else if (is_assignment(right)) 
            check_assignment(global_table, method_table, right);
    }

    /*If only one child, check for valid expression (plus, minus, not, and length)*/
    if (!right){
        if (strcmp(expr->name, "Plus") == 0
            || strcmp(expr->name, "Minus") == 0)
            {
                /*Rule out string, String[], void and boolean*/
                if (strcmp(left->annotation, "int") != 0
                    && strcmp(left->annotation, "double") != 0)
                    {
                        if (strcmp(expr->name, "Plus") == 0){
                            printf("Line %d, col %d: Operator + cannot be applied to type %s\n",
                                    expr->line, expr->col, expr->annotation);
                        }

                        if (strcmp(expr->name, "Minus") == 0){
                            printf("Line %d, col %d: Operator - cannot be applied to type %s\n",
                                    expr->line, expr->col, expr->annotation);
                        }
                        expr->annotation = strdup("undef");
            }
                else expr->annotation = strdup(left->annotation);
            }

        else if (strcmp(expr->name, "Not") == 0){
            /*Only boolean type is accepted*/
            if (strcmp(left->annotation, "boolean") != 0){
                        printf("Line %d, col %d: Operator ! cannot be applied to type %s\n",
                                expr->line, expr->col, expr->annotation);
                        expr->annotation = strdup("undef");
            }
            else expr->annotation = strdup(left->annotation);
        }
        
        /*TODO: Applies to string too?*/
        else if (strcmp(expr->name, "Length") == 0){
            if (strcmp(left->annotation, "String[]") != 0){
                printf("Line %d, col %d: Operator %s cannot be applied to type %s\n",
                        expr->line, expr->col, expr->literal,
                        left->annotation);
            }
            expr->annotation = strdup("int");
        }
    }
    else{
        /*If right child has no annotation, check it as well*/
        if (!right->annotation){
            if (is_expr(right)) 
                check_expression(global_table, method_table, right);
            else if (is_assignment(right)) 
                check_assignment(global_table, method_table, right);
            else if (is_statement(right))
                check_statement(global_table, method_table, right);
            else if (strcmp(right->name, "Id") == 0){
                symbol_t* symbol = find_symbol(method_table, right->type, right, false);
                if (!symbol) right->annotation = strdup("undef");
                else right->annotation = strdup(symbol->type);
            }
        }

        /*Check if this expression is valid (and annotate it)*/
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
            if (left->annotation && right->annotation)
            {
                /*Rule out string, String[], void and boolean*/
                if ((strcmp(left->annotation, "int") != 0
                    && strcmp(left->annotation, "double") != 0)
                    || (strcmp(right->annotation, "int") != 0
                    && strcmp(right->annotation, "double") != 0))
                    {
                        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                                    expr->line, expr->col,expr->literal,
                                    left->annotation, right->annotation);
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
            else {
                printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                            expr->line, expr->col,expr->literal,
                            left->annotation?left->annotation:"none",
                            right->annotation?right->annotation:"none");
                expr->annotation = strdup("undef");
            }
        }

    else if(strcmp(expr->name, "Xor") == 0
           || strcmp(expr->name, "And") == 0
           || strcmp(expr->name, "Or") == 0
           || strcmp(expr->name, "Not") == 0)
           {
                if (left->annotation && right->annotation){
                    /*Only boolean is accepted*/
                    if (strcmp(left->annotation, "boolean") != 0
                        || strcmp(right->annotation, "boolean") != 0)
                        {
                            printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                                    expr->line, expr->col,expr->literal,
                                    left->annotation, right->annotation);
                            expr->annotation = strdup("undef");
                        }

                    else expr->annotation = strdup("boolean");
                }
                else{
                    printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                            expr->line, expr->col,expr->literal,
                            left->annotation?left->annotation:"none", 
                            right->annotation?right->annotation:"none");
                    expr->annotation = strdup("undef");
                }
           }

    else if (strcmp(expr->name, "Lt") == 0
            || strcmp(expr->name, "Gt") == 0
            || strcmp(expr->name, "Le") == 0
            || strcmp(expr->name, "Ge") == 0
            || strcmp(expr->name, "Ne") == 0
            || strcmp(expr->name, "Eq") == 0)
            {
                // TODO: Allow void and String[]?
                if (left->annotation && right->annotation){
                    /*Both types must be equal*/
                    if (strcmp(left->annotation, right->annotation) != 0)
                    {
                        /*However, they may be compatible (int-double)*/
                        if ((strcmp(left->annotation, "int") == 0
                            && strcmp(right->annotation, "double") == 0)
                            || 
                            (strcmp(right->annotation, "int") == 0
                            && strcmp(left->annotation, "double") == 0))
                        {
                            expr->annotation = strdup("boolean");
                        }
                        else
                        {
                            printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                                    expr->line, expr->col,expr->literal,
                                    left->annotation, right->annotation);
                            expr->annotation = strdup("undef");
                        }
                        
                    }
                    else expr->annotation = strdup("boolean");
                }

                /*OR, one must be int/double and the other (L|R)shift*/
                else{
                    if ((right && strcmp(left->annotation, "undef") != 0
                            && strcmp(left->annotation, "void") != 0
                            && strcmp(left->annotation, "String[]") != 0
                            && strcmp(left->annotation, "string") != 0
                            && strcmp((right->name)+1, "shift") == 0)
                        || 
                        (left && strcmp(right->annotation, "undef") != 0
                            && strcmp(right->annotation, "void") != 0
                            && strcmp(right->annotation, "String[]") != 0
                            && strcmp(right->annotation, "string") != 0
                            && strcmp((left->name)+1, "shift") == 0))
                        {
                            printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                                    expr->line, expr->col,expr->literal,
                                    left->annotation?left->annotation:"none",
                                    right->annotation?right->annotation:"none");
                            expr->annotation = strdup("boolean");
                        }
                    else
                    {
                        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                                expr->line, expr->col,expr->literal,
                                left->annotation?left->annotation:"none", 
                                right->annotation?right->annotation:"none");
                        expr->annotation = strdup("undef");
                    }
                }
            }

    else if(strcmp(expr->name, "Lshift") == 0
            || strcmp(expr->name, "Rshift") == 0)
            {
                /*TODO: Only int is accepted?*/
                if (strcmp(left->annotation, "int") != 0
                    || strcmp(right->annotation, "int") != 0)
                    {
                        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                                expr->line, expr->col, expr->literal,
                                left->annotation, right->annotation);
                        expr->annotation = strdup("none");
                    }

                else{
                    /*Dont annotate anything but (L|R)shift*/
                    free(left->annotation);
                    free(right->annotation);
                    left->annotation = NULL;
                    right->annotation = NULL;
                }
            }
}

// FIXME: when one of children is undef, assign does not get undef
void check_assignment(table_t* global_table, table_t* method_table, node* assign){
    /*If Id is not annotated, annotate it*/
    node* assign_id = assign->son;
    if (!assign_id->annotation){
        symbol_t* found = find_symbol(method_table, assign_id->type, assign_id, false);
        if (found == NULL) assign_id->annotation = strdup("undef");
        else assign_id->annotation = strdup(found->type);
    }

    /*If right child is not annotated, check it as well*/
    node* right = assign->son->next;
    if (!right->annotation){
        if (is_expr(right)) check_expression(global_table, method_table, right);
        else if (is_assignment(right)) check_assignment(global_table, method_table, right);
        else if (is_statement(right)) check_statement(global_table, method_table, right);
    }

    /*Check assign statement*/
    if (!right->annotation){
        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                                assign->line, assign->col,assign->literal,
                                assign_id->annotation, "none");
        /*However, is left annotation int or double? If so, apply int, double to annotation*/
        if (strcmp(assign_id->annotation, "int") == 0
            || strcmp(assign_id->annotation, "double") == 0)
            {
                assign->annotation = strdup(assign_id->annotation);
            }
        else assign->annotation = strdup("undef");
    }
    else{
        /*If any of the annotations are undef, or they are do not match, assign error*/
        if (strcmp(assign_id->annotation, right->annotation) != 0
            || strcmp(assign_id->annotation, "undef") == 0
            || strcmp(right->annotation, "undef") == 0)
            {
                printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",
                                assign->line, assign->col,assign->literal,
                                assign_id->annotation, right->annotation);

                /*However, are any of the annotations int? If so, apply int to annotation*/
                if (strcmp(assign_id->annotation, "int") == 0
                    || strcmp(right->annotation, "int") == 0
                    || strcmp(assign_id->annotation, "double") == 0
                    || strcmp(right->annotation, "double") == 0)
                    {
                        assign->annotation = strdup(assign_id->annotation);
                    }
                else assign->annotation = strdup("undef");
            }

        else assign->annotation = strdup(assign_id->annotation);
    }
}

void check_block(table_t* global_table, table_t* method_table, node* stmt){
    node* temp = stmt->son;
    /*Travel through all the block's statements*/
    while(temp){
        check_statement(global_table, method_table, temp);
        temp = temp->next;
    }
}

void check_statement(table_t* global_table, table_t* method_table, node* stmt){
    if (is_expr(stmt)) check_expression(global_table, method_table, stmt);
    else if (is_assignment(stmt)) check_assignment(global_table, method_table, stmt);
    else if (strcmp(stmt->name, "Call") == 0) check_call(global_table, method_table, stmt);
    else if (strcmp(stmt->name, "Return") == 0) check_return(global_table, method_table, stmt);
    else if (strcmp(stmt->name, "If") == 0) check_if(global_table, method_table, stmt);
    else if (strcmp(stmt->name, "While") == 0) check_while(global_table, method_table, stmt);
    else if (strcmp(stmt->name, "Print") == 0) check_print(global_table, method_table, stmt);
    else if (strcmp(stmt->name, "ParseArgs") == 0)
        check_parse_args(global_table, method_table, stmt);
}

//FIXME: Call and parseargs have wrong col values
void check_return(table_t* global_table, table_t* method_table, node* return_node){
    /*Check child if not annotated yet*/
    node* child = return_node->son;
    /*Return void*/
    if (!child){
        /*Check if method is supposed to return something*/
        if (strcmp(method_table->return_type, "void") != 0){
            // FIXME: Wrong col number
            printf("Line %d, col %d: Incompatible type void in return statement\n", 
                    return_node->line, return_node->col);
        }

        return;
    }
    else if (!child->annotation){
        if (is_expr(child)) check_expression(global_table, method_table, child);
        else if (is_assignment(child)) check_assignment(global_table, method_table, child);
        else if (is_statement(child)) check_statement(global_table, method_table, child);
    }

    /*Check if child annotation matches with method return type, or*/
    /*if the method isn't supposed to return anything, and it has something to return*/
    if ((child->annotation && strcmp(child->annotation, method_table->return_type) != 0)
        || (strcmp(method_table->return_type, "void") == 0 && child)){
        if (strcmp(child->name, "Sub") == 0
           || strcmp(child->name, "Mul") == 0
           || strcmp(child->name, "Div") == 0
           || strcmp(child->name, "Mod") == 0
           || strcmp(child->name, "Add") == 0
           || strcmp(child->name, "Xor") == 0
           || strcmp(child->name, "And") == 0
           || strcmp(child->name, "Or") == 0
           || strcmp(child->name, "Not") == 0
           || strcmp(child->name, "Lt") == 0
           || strcmp(child->name, "Gt") == 0
           || strcmp(child->name, "Le") == 0
           || strcmp(child->name, "Ge") == 0
           || strcmp(child->name, "Ne") == 0
           || strcmp(child->name, "Eq") == 0
           || strcmp(child->name, "Lshift") == 0
           || strcmp(child->name, "Rshift") == 0
           || strcmp(child->name, "Minus") == 0
           || strcmp(child->name, "Plus") == 0)
           {
               // FIXME: Wrong col number
                printf("Line %d, col %d: Incompatible type %s in return statement\n",
                        child->line, child->col , child->annotation);

          }
        else{
            // FIXME: Wrong col number
            printf("Line %d, col %d: Incompatible type %s in return statement\n",
                    child->line, (int)(child->col - strlen(child->type)) , child->annotation);
        }
    }
}

void check_while(table_t* global_table, table_t* method_table, node* while_node){
    /*Check child node*/
    node* child = while_node->son->next;
    /*If left node has no annotation, check it as well*/
    if (!child->annotation){
        if (is_block_node(child)) 
            check_block(global_table, method_table, child);
        else if (is_statement(child)) 
            check_statement(global_table, method_table, child);
    }

    /*Check condition*/
    node* condition = while_node->son;
    /*If condition has no annotation, check it as well*/
    if (!condition->annotation){
        if (is_expr(condition)) 
            check_expression(global_table, method_table, condition);
        else if (is_assignment(condition))
            check_assignment(global_table, method_table, condition);
    }

    /*Semantically check the while statement, only boolean accepted in condition*/
    if (condition->annotation){
        if (strcmp(condition->annotation, "boolean") != 0){
            // FIXME: Wrong col number
            printf("Line %d, col %d: Incompatible type %s in while statement\n", 
                    while_node->line, while_node->col, while_node->annotation);
        }
    }
    // FIXME: Wrong col number
    else printf("Line %d, col %d: Incompatible type %s in while statement\n", 
                 while_node->line, while_node->col, "undef");

}

void check_parse_args(table_t* global_table, table_t* method_table, node* parse_args){
    /*Check if id annotation exists*/
    node* id_node = parse_args->son;
    if (!id_node->annotation){
        if (is_expr(id_node)) check_expression(global_table, method_table, id_node);
        else if (is_assignment(id_node)) check_assignment(global_table, method_table, id_node);
        else if (is_statement(id_node)) check_statement(global_table, method_table, id_node);
        else id_node->annotation = strdup("String[]");
    }

    /*Check if right child is annotated*/
    node* child = parse_args->son->next;
    if (!child->annotation){
        if (is_expr(child)) check_expression(global_table, method_table, child);
        else if (is_assignment(child)) check_assignment(global_table, method_table, child);
        else if (is_statement(child)) check_statement(global_table, method_table, child);
    }

    /*Check if parse args is valid*/
    if (strcmp(id_node->annotation, "String[]") != 0){
        // FIXME: Wrong col number
        printf("Line %d, col %d: Incompatible type %s in Integer.parseInt statement\n", 
                parse_args->line, parse_args->col, parse_args->annotation);
    }
    // FIXME: Wrong col values
    else if (strcmp(child->annotation, "int") != 0){
        printf("Line %d, col %d: Operator Integer.parseInt cannot be applied to types %s, %s\n", 
                parse_args->line,
                parse_args->col,
                id_node->annotation,
                child->annotation);
    }
    parse_args->annotation = strdup("int");
}

void check_print(table_t* global_table, table_t* method_table, node* print){
    /*Check if child annotation exists*/
    node* child = print->son;
    if (!child->annotation){
        if (is_expr(child)) check_expression(global_table, method_table, child);
        else if (is_assignment(child)) check_assignment(global_table, method_table, child);
        else if (is_statement(child)) check_statement(global_table, method_table, child);
    }

    //FIXME: Wrong col values
    /*Check if print is valid*/
    if (strcmp(child->annotation, "String[]") == 0
        || strcmp(child->annotation, "void") == 0 
        || strcmp(child->annotation, "undef") == 0){
        printf("Line %d, col %d: Incompatible type %s in System.out.print statement\n", 
                child->line, 
                child->son?((int)(child->col - strlen(child->son->type))):child->col, 
                child->annotation);
    }
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
           || strcmp(src->name, "Plus") == 0
           || strcmp(src->name, "Id") == 0
           || strcmp(src->name, "DecLit") == 0
           || strcmp(src->name, "BoolLit") == 0
           || strcmp(src->name, "RealLit") == 0
           || strcmp(src->name, "Length") == 0;
}

bool is_assignment(node* src){
    return strcmp(src->name, "Assign") == 0;
}

bool is_block_node(node* src){
    return strcmp(src->name, "Block") == 0;
}

bool is_statement(node* src){
    return strcmp(src->name, "If") == 0
           || strcmp(src->name, "While") == 0
           || strcmp(src->name, "Return") == 0
           || strcmp(src->name, "Print") == 0
           || strcmp(src->name, "Assign") == 0
           || strcmp(src->name, "Call") == 0
           || strcmp(src->name, "ParseArgs") == 0;
}
