#include "semantics.h"

#define _NONE 0

table_t* sem(node* tree){
    global = new_table("Global");
    // temp = vardecl ou funcdecl
    node* temp = tree->son;
    table_t* cur_table = global;
    while (temp){
        if (strcmp(temp->name, "FuncDecl") == 0){
            node* funcheader = temp->son;
            symbol_t* func = new_function_symbol(funcheader);
            insert_symbol(global, func);

            table_t* func_table;
            node* funcparams;
            if (strcmp(funcheader->son->next->name, "FuncParams") != 0){
                func_table = new_function_table(funcheader->son->name, funcheader->son->next->name, func->paramtypes);
                funcparams = funcheader->son->next->next;
            }
            else {
                func_table = new_function_table(funcheader->son->name, NULL, func->paramtypes);
                funcparams = funcheader->son->next;
            }
            cur_table->next = func_table;
            cur_table = cur_table->next;

            check_func_parameters(cur_table, funcparams->son); // funcparams->son = 1º paramdecl ou null
            check_func_body(cur_table, funcheader->next->son); // funcheader->next = funcbody->son = vardecl ou statement
            check_unused_symbols(cur_table);
        } else { // var decl global
            insert_symbol(global, new_vardecl(temp, false));
        }
        temp = temp->next;
    }
    check_unused_symbols(global);

    return global;
}

void check_func_parameters(table_t* table, node* cur_node){ // cur_node = paramdecl
    while (cur_node != NULL){
        insert_symbol(table, new_vardecl(cur_node, true));
        cur_node = cur_node->next;
    }
}

void check_func_body(table_t* table, node* cur_node){ // cur_node = vardecl ou statement
    if (cur_node == NULL) return;
    if (strcmp(cur_node->name, "VarDecl") == 0){
        insert_symbol(table, new_vardecl(cur_node, false));
        check_func_body(table, cur_node->next);
    } else {
        check_func_body(table, cur_node->son);
        check_func_body(table, cur_node->next);
        check_type(table, cur_node);
        check_statement(table, cur_node);
    }
}

bool starts_with(char* start, char* str){
    return strncmp(start, str, strlen(start)) == 0;
}

void check_return_statement(node* cur_node, char* func_return, char* expr_return){
    if (func_return != expr_return)
        printf("Line %d, column %d: Incompatible type %s in return statement\n", cur_node->line, cur_node->col, expr_return);
}

void check_if_for_statement(node* cur_node, char* s, char* expr_type){
    if (expr_type != "boolean")
        printf("Line %d, column %d: Incompatible type %s in %s statement\n", cur_node->line, cur_node->col, expr_type, s);
}

void check_assign_statement(node* cur_node, char* expr1_type, char* expr2_type){
    if (expr1_type != expr2_type || expr1_type == "Undefined" || expr2_type == "Undefined"){
        printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", cur_node->line, cur_node->col, expr1_type, expr2_type);
        cur_node->type = "Undefined";
    } else {
        cur_node->type = expr1_type;
    }
}

void check_call_statement(symbol_t* func, node* cur_node){
    if (func == NULL){
        cur_node->type = "Undefined";
    } else {
        int len_funcparams = 0; paramtypes_t* p = func->paramtypes;
        while (p != NULL){ len_funcparams++; p = p->next; }
        int len_callparams = 0; node* p2 = cur_node->son->next;
        while (p2 != NULL){ len_callparams++; p2 = p2->next; }
        if (len_callparams != len_funcparams){
            get_id(cur_node->son->name);
            printf("Line %d, column %d: Cannot find symbol %s\n", cur_node->son->line, cur_node->son->col, aux);
            cur_node->type = "Undefined";
        } else {
            bool erro;
            p = func->paramtypes; p2 = cur_node->son->next;
            while (p2 != NULL){
                if (p2->type != p->type){
                    printf("Line %d, column %d: Incompatible type %s in %s statement\n", p2->line, p2->col, p2->type, func->name);
                    erro = true;
                }
                p2 = p2->next;
                p = p->next;
            }
            if (erro){ cur_node->type = "Undefined"; }
            else { cur_node->type = func->type; }
        }
    }
}

void check_parseargs_statement(node* cur_node, node* expr1, node* expr2){
    if (expr1->type == "int"){
        if (expr2->type == "int"){
            expr1->type = "int";
        } else {
            printf("Line %d, column %d: Incompatible type %s in strconv.Atoi statement\n", expr2->line, expr2->col, expr2->type);
            cur_node->type = "Undefined";
        }
    } else {
        printf("Line %d, column %d: Incompatible type %s in strconv.Atoi statement\n", expr1->line, expr1->col, expr1->type);
        cur_node->type = "Undefined";
    }
}

void check_binary_expr(node* cur_node, char* s, char* expr1_type, char* expr2_type){
    if (expr1_type == "int" && expr2_type == "int")
        cur_node->type = "int";
    else if (expr1_type == "Double" && expr2_type == "Double")
        cur_node->type = "Double";
    else {
        printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
        cur_node->line, cur_node->col, s, expr1_type, expr2_type);
        cur_node->type = "Undefined";
    }
}

void check_plus_minus_expr(node* cur_node, char* s, char* expr_type){
    if (expr_type != "int" && expr_type != "Double"){
        printf("Line %d, column %d: Operator %s cannot be applied to type %s\n", cur_node->line, cur_node->col, s, expr_type);
        cur_node->type = "Undefined";
    } else {
        cur_node->type = expr_type;
    }
}

void check_not_expr(node* cur_node, char* expr_type){
    if (expr_type == "boolean"){
        cur_node->type = "boolean";
    } else {
        printf("Line %d, column %d: Operator ! cannot be applied to type %s\n", cur_node->line, cur_node->col, expr_type);
        cur_node->type = "Undefined";
    }
}

void check_logical_expr(node* cur_node, char* s, char* expr1_type, char* expr2_type){
    if (expr1_type == "boolean" && expr2_type == "boolean") {
        cur_node->type = "boolean";
    } else {
        printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
        cur_node->line, cur_node->col, s, expr1_type, expr2_type);
        cur_node->type = "Undefined";
    }
}

void check_comp_expr(node* cur_node, char* s, char* expr1_type, char* expr2_type){
    if (expr1_type == expr2_type){
        cur_node->type = "boolean";
    } else {
        cur_node->type = "Undefined";
        printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", cur_node->line, cur_node->col, s, expr1_type, expr2_type);
        cur_node->type = "Undefined";
    }
}

void check_statement(table_t* table, node* cur_node){
    if (strcmp(cur_node->name, "Return") == 0){
        if (cur_node->son == NULL){
            check_return_statement(cur_node, table->first->type, _NONE);
        } else {
            check_return_statement(cur_node, table->first->type, cur_node->son->type);
        }
    } else if (strcmp(cur_node->name, "If") == 0){
        check_if_for_statement(cur_node, "if", cur_node->son->type);
    } else if (strcmp(cur_node->name, "For") == 0){
        check_if_for_statement(cur_node, "for", cur_node->son->type);
    } else if (strcmp(cur_node->name, "Assign") == 0){
        check_assign_statement(cur_node, cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Call") == 0){
        get_id(cur_node->son->name);
        check_call_statement(find_symbol(global, aux), cur_node);
    } else if (strcmp(cur_node->name, "ParseArgs") == 0){
        check_parseargs_statement(cur_node, cur_node->son, cur_node->son->next);
    } else if (strcmp(cur_node->name, "Add") == 0){
        check_binary_expr(cur_node, "+", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Sub") == 0){
        check_binary_expr(cur_node, "-", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Mul") == 0){
        check_binary_expr(cur_node, "*", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Div") == 0){
        check_binary_expr(cur_node, "/", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Mod") == 0){
        check_binary_expr(cur_node, "%", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Plus") == 0){
        check_plus_minus_expr(cur_node, "+", cur_node->son->type);
    } else if (strcmp(cur_node->name, "Minus") == 0){
        check_plus_minus_expr(cur_node, "-", cur_node->son->type);
    } else if (strcmp(cur_node->name, "Not") == 0){
        check_not_expr(cur_node, cur_node->son->type);
    } else if (strcmp(cur_node->name, "And") == 0){
        check_logical_expr(cur_node, "&&", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Or") == 0){
        check_logical_expr(cur_node, "||", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Lt") == 0){
        check_comp_expr(cur_node, "<", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Gt") == 0){
        check_comp_expr(cur_node, ">", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Eq") == 0){
        check_comp_expr(cur_node, "==", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Ne") == 0){
        check_comp_expr(cur_node, "!=", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Le") == 0){
        check_comp_expr(cur_node, "<=", cur_node->son->type, cur_node->son->next->type);
    } else if (strcmp(cur_node->name, "Ge") == 0){
        check_comp_expr(cur_node, ">=", cur_node->son->type, cur_node->son->next->type);
    }
}

bool check_int(node* n){
    if (n->name[7] != '0') return true;
    int len = strlen(n->name) - 7;
    char temp[len];
    strcpy(temp, n->name+7);
    temp[len-1] = '\0';
    for (int i = 0; i < len; i++){
        if (temp[i] == '8' || temp[i] == '9'){
            printf("Line %d, column %d: Invalid octal constant: %s\n", n->line, n->col, temp);
            return false;
        }
    }
    return true;
}

void check_type(table_t* table, node* cur_node){
    if (starts_with("Id", cur_node->name)){
        get_id(cur_node->name);
        symbol_t* s = find_symbol(table, aux);
        if (s != NULL){
            cur_node->type = s->type;
            s->used = true;
        } else {
            printf("Line %d, column %d: Cannot find symbol %s\n", cur_node->line, cur_node->col, aux);
            cur_node->type = "Undefined";
        }
    } else if (starts_with("IntLit", cur_node->name)){
        if (check_int(cur_node)) {
            cur_node->type = "int";
        } else {
            cur_node->type = "Undefined";
        }
    } else if (starts_with("RealLit", cur_node->name)){
        cur_node->type = "Double";
    } else if (starts_with("StrLit", cur_node->name)){
        cur_node->type = "String";
    }
}

void check_unused_symbols(table_t* table){
    symbol_t* s = table->first->next;
    while (s != NULL){
        if (!(s->used)){
            printf("Line %d, column %d: Symbol %s declared but never used\n", s->line, s->col, s->name);
        }
        s = s->next;
    }
}

symbol_t* new_vardecl(node* vardecl, bool param){
    get_id(vardecl->son->next->name);
    return new_var_symbol(aux, vardecl->son->name, param, vardecl->son->next->line, vardecl->son->next->col);
}
