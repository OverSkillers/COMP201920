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


%type <no> Program ProgramRep MethodDecl FieldDecl FieldDeclRep MethodHeader FormalParams FormalParamsRep MethodBody MethodBodyRep VarDeclRep Statement StatementRep MethodInvocation ParseArgs Expr Type
assign ExprRep Statementaux
%type <vardec> VarDecl

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
    struct type_vardec* vardec;
	char* t;
}

%%

Program: CLASS ID LBRACE ProgramRep RBRACE { }

        ;

ProgramRep: {}

        | MethodDecl ProgramRep {}

        | FieldDecl ProgramRep {}

        | SEMICOLON ProgramRep {}

        ;

MethodDecl: PUBLIC STATIC MethodHeader MethodBody {}

        ;

FieldDecl: PUBLIC STATIC Type ID FieldDeclRep SEMICOLON {}

        | error SEMICOLON {$$ = NULL;/*printf("FieldDeclError\n");*/}

        ;

FieldDeclRep: {}

            |COMMA ID FieldDeclRep {}

            ;

Type: BOOL {}

    |INT {}

    |DOUBLE {}

    ;

MethodHeader: {}

            | Type ID LPAR FormalParams RPAR {}

            | VOID ID LPAR FormalParams RPAR {}

            ;

FormalParams: {}

            | Type ID FormalParamsRep {}

            | STR LSQ RSQ ID {}

            ;

FormalParamsRep: {}

            | COMMA Type ID FormalParamsRep {}

            ;

MethodBody: LBRACE MethodBodyRep RBRACE {}

            ;

MethodBodyRep: {}

            | Statement MethodBodyRep {}

            | VarDecl MethodBodyRep {}

            ;

VarDecl: Type ID VarDeclRep SEMICOLON {}

        ;

VarDeclRep: {}

            |COMMA ID VarDeclRep {}

            ;

Statement: LBRACE StatementRep RBRACE {}

         | IF LPAR assign RPAR Statement {}

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

     |ID {}

     |ID DOTLENGTH {}

     |INTLIT {/*printf("INTLIT\n");*/}

     |REALLIT {}

     |BOOLLIT {}

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
