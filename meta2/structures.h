#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct n{
	char *name;
	struct n* son;
	struct n* next;
} node;



/**
 * Enum defining the type of the expression, addition, subtraction, left shift, ...
 * */
typedef enum {t_add, t_sub, t_mul, t_div, t_mod,t_or, t_and, t_lt, t_le, t_gt, t_ge, t_ne, t_eq, t_lshift, t_rshift, t_xor, t_minus, t_plus, t_length} t_expr;


/**
 * Define an expression and its list
 * */
typedef struct type_expression{
    t_expr type;
    char* left, right;
} expression;

typedef struct type_expression_list{
    expression* data;
    struct type_expression_list* next;
} expression_list;





/**
 * Define a method invocation and its list
 */
typedef struct type_method_invoc{
    expression_list* args;
    char* id;
} method_invoc;

typedef struct type_method_invoc_list{
    method_invoc* data;
    struct type_method_invoc_list* next;
} method_invoc_list;






/**
 * Define different types of statements, assign statements, print, function call, ...
 * */
typedef struct type_statement_assign{
    char* id;
    expression* expression;
} statement_assign;


typedef struct type_statement_parseargs{
    char* id;
    expression* expression;
} statement_parseargs;


typedef struct type_statement_print{
    char* data;
} statement_print;


typedef struct type_statement_call{
    method_invoc_list* calls;
} statement_call;


typedef struct type_statement_return{
    expression* data;
} statement_return;


typedef struct type_statement_while{
    expression* condition;
    statement_list* body;
} statement_while;


typedef struct type_statement_if{
    expression* condition;
    statement_list* if_branch;
    statement_list* else_branch;
} statement_if;





/**
 * Enum defining the type of statement
 */
typedef enum {t_if, t_while, t_return, t_call, t_print, t_parseargs, t_assign, t_block} t_statement;

/**
 * Define a statement, can be of several different types of statement
 * Also define its list, and a block, which is only a list of statements
*/
typedef struct type_statement{
    t_statement type;
    union{
        statement_if* if_stmt;
        statement_while* while_stmt;
        statement_return* return_stmt;
        statement_call* call_stmt;
        statement_print* print_stmt;
        statement_parseargs* parseargs_stmt;
        statement_assign* assign_stmt;
    } data;
} statement;

typedef struct type_statement_list{
    statement* data;
    struct type_statement_list* next;
} statement_list;


typedef struct type_statement_block{
    t_statement type;
    statement_list* statements;
} statement_block;







/**
 * Define a function parameter declaration, and its list
 */
typedef struct type_param_decl{
    t_var type;
    char* id;
} param_decl;

typedef struct type_param_decl_list{
    param_decl* data;
    struct type_param_decl_list* next;
} param_decl_list;




/**
 * Define a method parameter node, which has a list of parameter declarations
*/
typedef struct type_method_params_list{
    param_decl_list* param_decls;
    struct type_method_params_list* next;
} method_params_list;




/**
 * Define a method header
*/
typedef struct type_method_header{
    t_var type;
    char* id;
    method_params_list* params;
} method_header;




/**
 * Define a method body
*/
typedef struct type_method_body{
    vardecl_list* decls;
    statement_list* statements;
} method_body;






/**
 * Define a method declaration, and its list
 */
typedef struct type_method_decl{
    method_header* header;
    method_body* body;
} method_decl;


typedef struct type_method_decl_list{
    method_decl* data;
    struct type_method_decl_list* next;
} method_decl_list;




/**
 * Enum defining the type of a variable
 * */
typedef enum {t_int, t_char, t_double, t_void, t_stringarray} t_var;


/**
 * Define a field declaration, and its list
*/
typedef struct type_field_decl{
    t_var type;
    char* id;
} field_decl;


typedef struct type_field_decl_list{
    field_decl* data;
    struct type_field_decl_list* next;
} field_decl_list;






/**
 * Define a VarDecl, variable declaration, and its list
 * 
 **/
typedef struct type_vardecl{
    t_var var_type;
    union{
        INT* int_dec;
        BOOL* bool_dec;
        DOUBLE* double_dec;
    } var_data;
} vardecl;


typedef struct type_vardecl_list{
    vardecl* data;
    struct type_vardecl_list* next;
} vardecl_list;






/**
 * Root node of the tree
*/
typedef struct type_program{
    char* id;
    field_decl_list* field_decl_l;
    method_decl_list* method_decl_l;
} program;






/**
 * Define a boolean terminal
 * */
typedef struct type_bool_term{
    char *id;
    int value;
} BOOL;


/**
 * Define an integer terminal
 * */
typedef struct type_int_term{
    char *id;
    int value;
} INT;


/**
 * Define a double terminal
 * */
typedef struct type_double_term{
    char *id;
    double value;
} DOUBLE;


#endif