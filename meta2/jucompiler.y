%{
#define MAX_STR 1024
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
#include "ast.h"

extern int total_lines,total_columns,error_line_number_String,error_line_column_String,
error_line_number_Comment,error_line_column_Comment;
extern char* yytext;
int yylex(void);
void yyerror (char *s);


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
%token STRING
%token VOID
%token WHILE
%token <s> RESERVED INTLIT REALLIT ID STRLIT BOOLLIT


%type <no> Program ProgramRep MethodDecl FieldDecl FieldDeclRep MethodHeader FormalParams FormalParamsRep MethodBody MethodBodyRep VarDecl VarDeclRep Statement StatementRep MethodInvocation MethodInvocationRep Assignment ParseArgs Expr Type

%right NOT
%left DIV MOD STAR
%left PLUS MINUS
%left RSHIFT LSHIFT
%left GE GT LT LE
%left EQ NE
%left XOR
%left AND
%left OR
%right ASSIGN


%union{
	char* s;
	struct n* no;
	char* t;
}

%%

Program: CLASS ID LBRACE RBRACE { }

        | CLASS ID LBRACE ProgramRep RBRACE { }

        ;

ProgramRep: MethodDecl {}

        | MethodDecl ProgramRep {}

        | FieldDecl {}

        | FieldDecl ProgramRep {}

        | SEMICOLON {}

        | SEMICOLON ProgramRep {}

        ;

MethodDecl: PUBLIC STATIC MethodHeader MethodBody {}

        ;

FieldDecl: PUBLIC STATIC Type ID SEMICOLON {}

        | PUBLIC STATIC Type ID FieldDeclRep SEMICOLON {}

        ;

FieldDeclRep: COMMA ID {}

            |COMMA ID FieldDeclRep {}

            |error SEMICOLON {$$ = NULL;}

            ;

Type: BOOL {}

    |INT {}

    |DOUBLE {}

    ;

MethodHeader: Type ID LPAR RPAR {}

            | Type ID LPAR FormalParams RPAR {}

            | VOID ID LPAR RPAR {}

            | VOID ID LPAR FormalParams RPAR {}

            ;

FormalParams: Type ID {}

            | Type ID FormalParamsRep {}

            | STRING LSQ RSQ ID {}

            ;

FormalParamsRep: COMMA Type ID {}

            | COMMA Type ID FormalParamsRep {}

            ;

MethodBody: LBRACE RBRACE {}

            |LBRACE MethodBodyRep RBRACE {}

            ;

MethodBodyRep: Statement VarDecl {}

            | Statement VarDecl MethodBodyRep {}

            ;

VarDecl: Type ID SEMICOLON {}

        |Type ID VarDeclRep SEMICOLON {}

        ;

VarDeclRep: COMMA ID {}

            |COMMA ID VarDeclRep {}

            ;

Statement: LBRACE RBRACE {}

         | LBRACE StatementRep RBRACE {}

         | IF LPAR Expr RPAR Statement {}

         | IF LPAR Expr RPAR Statement ELSE Statement {}

         | WHILE LPAR Expr RPAR Statement {}

         | RETURN SEMICOLON {}

         | RETURN Expr SEMICOLON {}

         | SEMICOLON {}

         | MethodInvocation SEMICOLON {}

         | Assignment SEMICOLON {}

         | ParseArgs SEMICOLON {}

         | PRINT LPAR Expr RPAR SEMICOLON {}

         | PRINT LPAR STRLIT RPAR SEMICOLON {}

         | error SEMICOLON {$$ = NULL;}

         ;

StatementRep: Statement {}

        ;

MethodInvocation: ID LPAR RPAR {}

        | ID LPAR Expr RPAR {}

        | ID LPAR Expr MethodInvocationRep RPAR {}

        ;

MethodInvocationRep: COMMA Expr MethodInvocationRep {}

                    |ID LPAR error RPAR {$$ = NULL;}

        ;

Assignment: ID ASSIGN Expr {}

        ;

ParseArgs: PARSEINT LPAR ID LSQ Expr RSQ LPAR {}

          |PARSEINT LPAR error RPAR {$$ = NULL;}

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

     |LPAR Expr RPAR {}

     |MethodInvocation {}

     |Assignment {}

     |ParseArgs {}

     |ID {}

     |ID DOTLENGTH {}

     |INTLIT {}

     |REALLIT {}

     |BOOLLIT {}

     |LPAR error RPAR {$$ = NULL;}

    ;
%%
