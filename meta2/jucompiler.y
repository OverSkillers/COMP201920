%{
#ifndef DEBUG
#define DEBUG 0
#endif

#define MAX_STR 1024
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
#include "ast.h"
#include "structures.h"

extern int yylineno,coluna,yyleng;
extern int total_lines,total_columns,error_line_number_String,error_line_column_String,
error_line_number_Comment,error_line_column_Comment,linha;
extern char* yytext;
int yylex(void);
void yyerror (char *s);

extern program* ast_root;

//TODO: Free these helper lists whenever they are set to NULL
field_decl_list* d_field_decl_list = NULL;
field_decl_ids* temp_field_decl_ids = NULL;
method_decl_list* d_method_decl_list = NULL;
param_decl_list* temp_param_list = NULL;
method_body_nodes* temp_method_body_nodes = NULL;
vardecl_list* temp_vardecl_list = NULL;
vardecl_ids* temp_vardecl_ids = NULL;
statement_list* temp_stmt_list = NULL;
expression_list* expr_list;

//For debug purposes
#if (DEBUG > 0)
int yydebug=1;
#endif



%}

%token AND
%token ASSIGN
%token STAR
%token COMMA
%token DIV
%token EQ
%token GE
%token LBRACE
%token LE
%token LPAR
%token LSQ
%token LT
%token MINUS
%token MOD
%token NE
%token NOT
%token OR
%token PLUS
%token RBRACE
%token RPAR
%token RSQ
%token SEMICOLON
%token ARROW
%token LSHIFT
%token RSHIFT
%token XOR
%token BOOL
%token CLASS
%token DOTLENGTH
%token DOUBLE
%token ELSE
%token IF
%token INT
%token PRINT
%token PARSEINT
%token PUBLIC
%token RETURN
%token STATIC
%token STR
%token VOID
%token WHILE
%token <s> RESERVED INTLIT REALLIT ID STRLIT BOOLLIT


%type <no> Program ProgramRep FieldDeclRep VarDeclRep StatementRep MethodInvocation ParseArgs
%type <vardecl_list> VarDecl
%type <expression> Expr
%type <method_decl> MethodDecl
%type <method_header> MethodHeader
%type <method_body> MethodBody
%type <param_decl_list> FormalParams FormalParamsRep
%type <field_decl> FieldDecl
%type <statement_list> Statement

%type <t> Type

%right ASSIGN
%left OR
%left AND
%left XOR
%left EQ NE
%left GE GT LT LE
%left RSHIFT LSHIFT
%left PLUS MINUS
%left DIV MOD STAR
%right NOT
%left LPAR RPAR LSQ RSQ

%nonassoc IF
%nonassoc ELSE


%union{
	char* s;
	struct n* no;
        struct type_vardecl_list* vardecl_list;
        struct type_expression* expression;
        struct type_method_decl* method_decl;
        struct type_method_header* method_header;
        struct type_method_body* method_body;
        struct type_method_body_nodes* method_body_nodes;
        struct type_param_decl_list* param_decl_list;
        struct type_field_decl* field_decl;
        struct type_statement_list* statement_list;
	int t;
}

%%

Program: CLASS ID LBRACE ProgramRep RBRACE { ast_root = insert_program($2, d_field_decl_list, d_method_decl_list);}

        ;

ProgramRep: {}

        | MethodDecl ProgramRep {d_method_decl_list = insert_method_decl(d_method_decl_list, $1);}

        | FieldDecl ProgramRep {}

        | SEMICOLON ProgramRep {}

        ;

MethodDecl: PUBLIC STATIC MethodHeader MethodBody {$$ = create_method_decl($3, $4);}

        ;

FieldDecl: PUBLIC STATIC Type ID FieldDeclRep SEMICOLON {
                                                                temp_field_decl_ids = insert_field_decl_id(temp_field_decl_ids, $4); 
                                                                d_field_decl_list = insert_field_decl(d_field_decl_list, $3, temp_field_decl_ids);
                                                                temp_field_decl_ids = NULL;
                                                        }

        | error SEMICOLON {$$ = NULL;/*printf("FieldDeclError\n");*/}

        ;

FieldDeclRep: {}

            |COMMA ID FieldDeclRep { temp_field_decl_ids = insert_field_decl_id(temp_field_decl_ids, $2); }

            ;

Type: BOOL {$$ = t_bool;}

    |INT {$$ = t_int;}

    |DOUBLE {$$ = t_double;}

    ;

MethodHeader: {}

            | Type ID LPAR FormalParams RPAR {$$ = create_method_header($1, $2, $4); temp_param_list = NULL;}

            | VOID ID LPAR FormalParams RPAR {$$ = create_method_header(t_void, $2, $4); temp_param_list = NULL;}

            ;

FormalParams: {}

            | Type ID FormalParamsRep {$$ = temp_param_list = insert_param_decl(temp_param_list, create_param_decl($1, $2));}

            | STR LSQ RSQ ID {$$ = temp_param_list = insert_param_decl(temp_param_list, create_param_decl(t_stringarray, $4));}

            ;

FormalParamsRep: {}

            | COMMA Type ID FormalParamsRep {temp_param_list = insert_param_decl(temp_param_list, create_param_decl($2, $3));}

            ;

MethodBody: LBRACE MethodBodyRep RBRACE {$$ = create_method_body(temp_method_body_nodes); temp_method_body_nodes=NULL;}

            ;

MethodBodyRep: {}

            | Statement MethodBodyRep {temp_method_body_nodes = insert_mbody_statement(temp_method_body_nodes, $1);}

            | VarDecl MethodBodyRep {temp_method_body_nodes = insert_mbody_vardecl(temp_method_body_nodes, $1);}

            ;

VarDecl: Type ID VarDeclRep SEMICOLON {
                                        temp_vardecl_ids = insert_vardecl_id(temp_vardecl_ids, $2);
                                        $$ = temp_vardecl_list = insert_vardecl(temp_vardecl_list, $1, temp_vardecl_ids);
                                        temp_vardecl_ids = NULL;
                                        temp_vardecl_list = NULL;
                                      }

        ;

VarDeclRep: {}

            |COMMA ID VarDeclRep {temp_vardecl_ids = insert_vardecl_id(temp_vardecl_ids, $2);}

            ;

Statement: LBRACE StatementRep RBRACE {}

         | IF LPAR assign RPAR Statement {/*$$ = temp_stmt_list = insert_statement(temp_stmt_list, create_if_statement($3, $5, NULL));*/}

         | IF LPAR assign RPAR Statement ELSE Statement {}

         | WHILE LPAR assign RPAR Statement {}

         | RETURN SEMICOLON {}

         | RETURN assign SEMICOLON {}

         | SEMICOLON {}

         | Statementaux SEMICOLON {}

         | PRINT LPAR assign RPAR SEMICOLON {}

         | PRINT LPAR STRLIT RPAR SEMICOLON {}

         | error SEMICOLON {$$ = NULL;/*printf("StatementError\n");*/}

         ;


StatementRep:   {}

         | Statement StatementRep {}

        ;

Statementaux: ParseArgs {}

        | MethodInvocation {}

        | ID ASSIGN assign {}

        ;

MethodInvocation: ID LPAR RPAR {}

        | ID LPAR assign ExprRep RPAR {}

        | ID LPAR error RPAR {$$ = NULL;/*printf("MethodInvocationError\n");*/}

        ;

ExprRep: {}

        | ExprRep COMMA assign {}

        ;


ParseArgs: PARSEINT LPAR ID LSQ assign RSQ RPAR {}

          |PARSEINT LPAR error RPAR {$$ = NULL;/*printf("ParseArgsError\n");*/}

        ;

Expr: Expr PLUS Expr {}

     |Expr MINUS Expr {}

     |Expr STAR Expr {}

     |Expr DIV Expr {}

     |Expr MOD Expr {}

     |Expr AND Expr {}

     |Expr OR Expr {}

     |Expr XOR Expr {}

     |Expr LSHIFT Expr {}

     |Expr RSHIFT Expr {}

     |Expr EQ Expr {}

     |Expr GE Expr {}

     |Expr GT Expr {}

     |Expr LE Expr {}

     |Expr LT Expr {}

     |Expr NE Expr {}

     |MINUS Expr {} %prec NOT

     |NOT Expr {} %prec NOT

     |PLUS Expr {} %prec NOT

     |LPAR assign RPAR {}

     |MethodInvocation {}

     |ParseArgs {}

     |ID {/*$$ = create_expression(t_strlit, )*/}

     |ID DOTLENGTH {}

     |INTLIT { $$ = insert_expression_node(t_intlit, strdup($1), NULL); }

     |REALLIT { $$ = insert_expression_node(t_reallit, strdup($1), NULL);}

     |BOOLLIT { $$ = insert_expression_node(t_boollit, strdup($1), NULL);}

     |LPAR error RPAR {$$ = NULL;/*printf("ExprError\n");*/}

     ;

assign: ID ASSIGN assign {}

    | LPAR ID ASSIGN assign RPAR {}

    | Expr {}
    ;


%%

void yyerror (char *s){
	// Como STRLIT é um estado do lex o yytext apenas contem a aspa de fechar a string, por isso tem de se usar yylval em vez de yytext
	if (yychar == STRLIT){
		printf("Line %d, col %d: %s: \"%s\"\n", total_lines, (int) (total_columns-strlen(yylval.s)) - 2, s, yylval.s);
	} else if ((yytext[strlen(yytext) - 1] == '\n') || (yytext[strlen(yytext) - 1] == '\r') || (strcmp(yytext,"\r\n")==0) || yychar == 0){
		printf("Line %d, col %d: %s: %s\n", total_lines, total_columns , s, yytext);
	} else {
		printf("Line %d, col %d: %s: %s\n", total_lines, total_columns-yyleng, s, yytext);
	}
}
