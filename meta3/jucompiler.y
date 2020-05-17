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
#include "symbol.h"

extern int yylineno,coluna,yyleng;
extern int total_lines,total_columns,error_line_number_String,error_line_column_String,
error_line_number_Comment,error_line_column_Comment,linha;
extern char* yytext;
int yylex(void);
void yyerror (char *s);
int erro = 0;
int erroField = 0;


struct n* tree;

//For debug purposes
#if (DEBUG == 1)
int yydebug=1;
#endif


struct type_struct {
	int line, col;
	const char* type;
};


%}

%union {
	struct n* no;
	struct type_struct* t;
	struct _args* args;
};

%token <args> AND
%token <args> ASSIGN
%token <args> STAR
%token <args> COMMA
%token <args> DIV
%token <args> EQ
%token <args> GE
%token <args> GT
%token <args> LBRACE
%token <args> LE
%token <args> LPAR
%token <args> LSQ
%token <args> LT
%token <args> MINUS
%token <args> MOD
%token <args> NE
%token <args> NOT
%token <args> OR
%token <args> PLUS
%token <args> RBRACE
%token <args> RPAR
%token <args> RSQ
%token <args> SEMICOLON
%token <args> ARROW
%token <args> LSHIFT
%token <args> RSHIFT
%token <args> XOR
%token <args> BOOL
%token <args> CLASS
%token <args> DOTLENGTH
%token <args> DOUBLE
%token <args> ELSE
%token <args> IF
%token <args> INT
%token <args> PRINT
%token <args> PARSEINT
%token <args> PUBLIC
%token <args> RETURN
%token <args> STATIC
%token <args> STR
%token <args> VOID
%token <args> WHILE
%token <args> RESERVED INTLIT REALLIT ID STRLIT BOOLLIT


%type <no> Program ProgramRep MethodDecl FieldDecl FieldDeclRep MethodHeader FormalParams FormalParamsRep MethodBody MethodBodyRep VarDeclRep Statement StatementRep MethodInvocation ParseArgs Expr Assignment ExprRep Statementaux Aux VarDecl Type

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

%%

Program: CLASS ID LBRACE ProgramRep RBRACE 
        {
                tree = create_node("Program", "NULL", getLine($1), getCol($1));
                node* temp = create_node("Id", $<args->val>2, getLine($2), getCol($2));
                add_next(temp, $4);
                add_son(tree, temp);
                $$ = tree;
        }

        ;

ProgramRep: {$$ = NULL;}

        | MethodDecl ProgramRep {add_next($1,$2); $$=$1;}

        | FieldDecl ProgramRep {add_next($1,$2); $$=$1;}

        | SEMICOLON ProgramRep {$$=$2;}

        ;

MethodDecl: PUBLIC STATIC MethodHeader MethodBody 
        {
                node* temp; 
                temp = create_node("MethodDecl", "NULL", getLine($1), getCol($1));
                add_son(temp, $3);
                add_son(temp, $4);
                /*printf("%s\n",temp->son->name);*/
                $$=temp;
        }

        ;

FieldDecl: PUBLIC STATIC Type ID FieldDeclRep SEMICOLON 
        {
                node* temp = create_node("FieldDecl", "NULL", getLine($4), getCol($4));
                node* temp1 = create_node("Id", $<args->val>4, getLine($4), getCol($4));
                add_son(temp, $3);
                add_son(temp, temp1);
                add_next(temp, $5);
                $$ = temp; 
                node* temp3 = temp;
                char* temp4 = temp->son->name;
                while (temp3->next != NULL)
                {
                        temp3 = temp3->next;
                        node* tipo = create_node(temp4, "NULL", getLineN($3), getColN($3));
                        add_next(tipo, temp3->son);
                        temp3->son = tipo;
                }
                $$ = temp;
        }

        | error SEMICOLON {$$ = NULL; erro = 1; /*printf("FieldDeclError\n");*/}

        ;

FieldDeclRep: {$$ = NULL;}

        | COMMA ID FieldDeclRep 
        {
                node* temp = create_node("FieldDecl", "NULL", getLine($2), getCol($2));
                node* temp1 = create_node("Id", $<args->val>2, getLine($2), getCol($2));
                add_son(temp, temp1);
                add_next(temp, $3);
                $$ = temp;
        }

        ;

Type: BOOL {node* temp = create_node("Bool", "NULL", getLine($1), getCol($1)); $$ = temp;}

        | INT {node* temp = create_node("Int", "NULL", getLine($1), getCol($1)); $$ = temp;}

        | DOUBLE {node* temp = create_node("Double", "NULL", getLine($1), getCol($1)); $$ = temp;}

        ;

MethodHeader: {$$ = NULL;}

        | Type ID LPAR FormalParams RPAR 
        {
                node* temp = create_node("MethodHeader", "NULL", getLine($2), getCol($2));
                node* temp1 = create_node("Id", $<args->val>2, getLine($2), getCol($2));
                node* temp2 = create_node("MethodParams", "NULL", getLine($2), getCol($2));
                add_son(temp2, $4);
                add_son(temp, $1);
                add_son(temp, temp1);
                add_son(temp, temp2);
                $$ = temp;
        }

        | VOID ID LPAR FormalParams RPAR 
        {
                node* temp = create_node("MethodHeader", "NULL", getLine($2), getCol($2));
                node* temp1 = create_node("Void", "NULL", getLine($1), getCol($1));
                node* temp2 = create_node("Id", $<args->val>2, getLine($2), getCol($2));
                node* temp3 = create_node("MethodParams", "NULL", getLine($2), getCol($2));
                add_son(temp3,$4);
                add_son(temp, temp1);
                add_son(temp, temp2);
                add_son(temp, temp3);
                $$=temp;
        }

        ;

FormalParams: {$$ = NULL;}

        | Type ID FormalParamsRep 
        {
                node* temp = create_node("ParamDecl", "NULL", getLine($2), getCol($2));
                node* temp1 = create_node("Id", $<args->val>2, getLine($2), getCol($2));
                add_son(temp, $1);
                add_son(temp, temp1);
                add_next(temp, $3);
                $$=temp;
        }

        | STR LSQ RSQ ID 
        {
                node* temp = create_node("ParamDecl", "NULL", getLine($3), getCol($3));
                node* temp1 = create_node("StringArray", "NULL", getLine($1), getCol($1));
                node* temp2 = create_node("Id", $<args->val>4, getLine($4), getCol($4));
                add_next(temp1, temp2);
                add_son(temp, temp1);
                $$ = temp;
        }

        ;

FormalParamsRep: {$$ = NULL;}

        | COMMA Type ID FormalParamsRep 
        {
                node* temp = create_node("ParamDecl", "NULL", getLine($3), getCol($3));
                node* temp1 = create_node("Id", $<args->val>3, getLine($3), getCol($3));
                add_son(temp, $2);
                add_son(temp, temp1);
                add_next(temp, $4);
                $$ = temp;
        }

        ;

MethodBody: LBRACE MethodBodyRep RBRACE 
        {
                node* temp = create_node("MethodBody", "NULL", getLine($1), getCol($1));
                add_son(temp, $2);
                $$ = temp;
        }

        ;

MethodBodyRep: {$$ = NULL;}

        | Statement MethodBodyRep 
        {
                node* temp = $1;
                if (temp == NULL) $$ = $2;
                else
                {
                        add_next(temp, $2);
                        $$ = temp;
                }
        }

        | VarDecl MethodBodyRep 
        {
                node* temp = $1;
                if (temp == NULL) $$ = $2;
                else
                {
                        add_next(temp, $2);
                        $$ = temp;
                }
        }

        ;

VarDecl: Type ID VarDeclRep SEMICOLON 
        {
                node* temp = create_node("VarDecl", "NULL", getLine($2), getCol($2));
                node* temp1 = create_node("Id", $<args->val>2, getLine($2), getCol($2));
                add_son(temp, $1);
                add_son(temp, temp1);
                add_next(temp, $3);
                node* temp3 = temp;
                char* temp4 = temp->son->name;
                while (temp3->next != NULL)
                {
                        temp3 = temp3->next;
                        node* tipo = create_node(temp4, "NULL", getLineN($1), getColN($1));
                        add_next(tipo, temp3->son);
                        temp3->son = tipo;
                }
                $$ = temp;
        }

        ;

VarDeclRep: {$$ = NULL;}

        |COMMA ID VarDeclRep 
        {
                node* temp = create_node("VarDecl", "NULL", getLine($2), getCol($2));
                node* temp1 = create_node("Id", $<args->val>2, getLine($2), getCol($2));
                add_son(temp, temp1);
                add_next(temp, $3);
                $$ = temp;
        }

        ;

Statement: LBRACE StatementRep RBRACE 
        {
                if (is_block($2) == true)
                {
                        node* temp = create_node("Block","NULL",getLineN($2),getColN($2));
                        add_son(temp, $2);
                        $$ = temp;
                }
                else $$ = $2;
        }

        | IF LPAR Aux RPAR Statement 
        {
                node* temp = create_node("If", "NULL", getLine($1), getCol($1));
                add_son(temp, $3);
                $$ = temp;
                if ($5 == NULL || is_block($5) == true)
                {
                        node* temp1 = create_node("Block", "NULL", getLineN($5), getColN($5));
                        add_son(temp1, $5);
                        add_son(temp, temp1);
                }
                else add_son($$, $5);
                node* temp2 = create_node("Block", "NULL", getLineN($5), getColN($5));
                add_son($$, temp2);
        }

        | IF LPAR Aux RPAR Statement ELSE Statement 
        {
                node* temp = create_node("If", "NULL", getLine($1), getCol($1));
                add_son(temp, $3);
                $$ = temp;
                if ($5 == NULL || is_block($5) == true)
                {
                        node* temp1 = create_node("Block", "NULL", getLineN($5), getColN($5));
                        add_son(temp1, $5);
                        add_son(temp, temp1);
                }
                else add_son($$, $5);

                if ($7 == NULL || is_block($7) == true)
                {
                        node* temp2 = create_node("Block", "NULL", getLineN($7), getColN($7));
                        add_son(temp2, $7);
                        add_son(temp, temp2);
                }
                else add_son($$, $7);
        }

        | WHILE LPAR Aux RPAR Statement 
        {
                node* temp = create_node("While", "NULL", getLine($1), getCol($1));
                add_son(temp, $3);
                $$=temp;
                if ($5 == NULL || is_block($5) == true)
                {
                        node* temp1 = create_node("Block", "NULL", getLineN($5), getColN($5));
                        add_son(temp1, $5);
                        add_son(temp, temp1);
                }
                else add_son($$,$5);
        }

        | RETURN SEMICOLON 
        {
                node* temp = create_node("Return", "NULL", getLine($1), getCol($1));
                $$ = temp;
        }

        | RETURN Aux SEMICOLON 
        {
                node* temp = create_node("Return", "NULL", getLine($1), getCol($1));
                add_son(temp, $2);
                $$ = temp;
        }

        | SEMICOLON {$$ = NULL;}

        | Statementaux SEMICOLON {$$ = $1;}

        | PRINT LPAR Aux RPAR SEMICOLON
        {
                node* temp = create_node("Print", "NULL", getLine($1), getCol($1));
                add_son(temp, $3);
                $$ = temp;
        }

        | PRINT LPAR STRLIT RPAR SEMICOLON 
        {
                node* temp = create_node("Print", "NULL", getLine($1), getCol($1));
                node* temp1 = create_node("StrLit", $<args->val>3, getLine($3), getCol($3));
                set_annotation(temp1, "string");
                add_son(temp, temp1);
                $$ = temp;
        }

        | error SEMICOLON {$$ = NULL; erro = 1; /*printf("StatementError\n");*/}

        ;


StatementRep:   {$$ = NULL;}

        | Statement StatementRep 
        {
                if ($1 == NULL) $$ = $2;
                else
                {
                        add_next($1,$2);
                        $$=$1;
                }
        }

        ;

Statementaux: ParseArgs {$$ = $1;}

        | MethodInvocation {$$ = $1;}

        | ID ASSIGN Aux 
        {
                node* temp1 = create_node("Assign", "NULL", getLine($2), getCol($2));
                node* temp2 = create_node("Id", $<args->val>1, getLine($1), getCol($1));
                set_literal_symbol(temp1, "=");
                add_son(temp1, temp2);
                add_son(temp1, $3);
                $$ = temp1;
        }

        ;

MethodInvocation: ID LPAR RPAR 
        {
                node* temp1 = create_node("Call", "NULL", getLine($1), getCol($1));
                node* temp2 = create_node("Id", $<args->val>1, getLine($1), getCol($1));
                add_son(temp1, temp2);
                $$ = temp1;
        }

        | ID LPAR Aux ExprRep RPAR 
        {
                node* temp1 = create_node("Call", "NULL", getLine($1), getCol($1));
                node* temp2 = create_node("Id", $<args->val>1, getLine($1), getCol($1));
                add_son(temp1, temp2);
                add_son(temp1, $3);
                add_son(temp1, $4);
                $$ = temp1;
        }

        | ID LPAR error RPAR {$$ = NULL; erro = 1;}

        ;

Assignment: ID ASSIGN Aux 
        {
                node* temp1 = create_node("Assign", "NULL", getLine($2), getCol($2));
                node* temp2 = create_node("Id", $<args->val>1, getLine($1), getCol($1));
                set_literal_symbol(temp1, "=");
                add_son(temp1, temp2);
                add_son(temp1, $3);
                $$ = temp1;
        }

        ;

ExprRep: {$$ = NULL;}

        | ExprRep COMMA Aux 
        {
                node* temp = $1;
                if (temp == NULL) $$ = $3;
                else
                {
                        add_next(temp,$3);
                        $$ = temp;
                }
        }

        ;


ParseArgs: PARSEINT LPAR ID LSQ Aux RSQ RPAR 
        {
                node* temp = create_node("ParseArgs", "NULL", getLine($3),getCol($3));
                //set_annotation(temp, "int");
                node* temp1 = create_node("Id", $<args->val>3, getLine($3), getCol($3));
                set_annotation(temp1, "String[]");
                add_son(temp, temp1);
                add_son(temp, $5);
                $$ = temp;
        }

        | PARSEINT LPAR error RPAR {$$ = NULL; erro = 1;}

        ;

Expr: Expr PLUS Expr 
        {
                node* temp = create_node("Add", "NULL", getLine($2), getCol($2));
                set_literal_symbol(temp, "+");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr MINUS Expr 
        {
                node* temp = create_node("Sub", "NULL", getLine($2), getCol($2));
                set_literal_symbol(temp, "-");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr STAR Expr 
        {
                node* temp = create_node("Mul", "NULL", getLine($2), getCol($2));
                set_literal_symbol(temp, "*");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr DIV Expr 
        {
                node* temp = create_node("Div", "NULL", getLine($2), getCol($2));
                set_literal_symbol(temp, "/");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr MOD Expr 
        {
                node* temp = create_node("Mod", "NULL", getLine($2), getCol($2));
                set_literal_symbol(temp, "%");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr AND Expr 
        {
                node* temp = create_node("And", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, "&&");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr OR Expr 
        {
                node* temp = create_node("Or", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, "||");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr XOR Expr 
        {
                node* temp = create_node("Xor", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, "^");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr LSHIFT Expr 
        {
                node* temp = create_node("Lshift", "NULL", getLine($2), getCol($2));
                set_literal_symbol(temp, "<<");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr RSHIFT Expr 
        {
                node* temp = create_node("Rshift", "NULL", getLine($2), getCol($2));
                set_literal_symbol(temp, ">>");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr EQ Expr 
        {
                node* temp = create_node("Eq", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, "==");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr GE Expr 
        {
                node* temp = create_node("Ge", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, ">=");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr GT Expr 
        {
                node* temp = create_node("Gt", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, ">");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr LE Expr 
        {
                node* temp = create_node("Le", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, "<=");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr LT Expr 
        {
                node* temp = create_node("Lt", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, "<");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | Expr NE Expr 
        {
                node* temp = create_node("Ne", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, "!=");
                add_son(temp, $1);
                add_son(temp, $3);
                $$ = temp;
        }

        | MINUS Expr 
        {
                node* temp = create_node("Minus", "NULL", getLine($1), getCol($1));
                //set_annotation(temp, $2->annotation);
                set_literal_symbol(temp, "-");
                add_son(temp, $2);
                $$=temp;
        } %prec NOT

        | NOT Expr 
        {
                node* temp = create_node("Not", "NULL", getLine($1), getCol($1));
                //set_annotation(temp, "boolean");
                set_literal_symbol(temp, "!");
                add_son(temp, $2);
                $$=temp;
        } %prec NOT

        | PLUS Expr 
        {
                node* temp = create_node("Plus", "NULL", getLine($1), getCol($1));
                //set_annotation(temp, $2->annotation);
                set_literal_symbol(temp, "+");
                add_son(temp, $2);
                $$=temp;
        } %prec NOT

        | LPAR Aux RPAR {$$ = $2;}

        | MethodInvocation {$$ = $1;}

        | ParseArgs {$$ = $1;}

        | ID 
        {
                node* temp = create_node("Id", $<args->val>1, getLine($1), getCol($1));
                $$ = temp;
        }

        | ID DOTLENGTH 
        {
                node* temp = create_node("Id", $<args->val>1, getLine($1), getCol($1));
                node* temp1 = create_node("Length", "NULL", getLine($2), getCol($2));
                //set_annotation(temp, "int");
                set_annotation(temp1, "int");
                set_literal_symbol(temp1, ".length");
                add_son(temp1, temp);
                $$ = temp1;
        }

        | INTLIT 
        {
                node* temp = create_node("DecLit", $<args->val>1, getLine($1), getCol($1));
                //set_annotation(temp, "int");
                $$ = temp;
        }

        | REALLIT 
        {
                node* temp = create_node("RealLit", $<args->val>1, getLine($1), getCol($1));
                //set_annotation(temp, "double");
                $$ = temp;
        }

        | BOOLLIT 
        {
                node* temp = create_node("BoolLit", $<args->val>1, getLine($1), getCol($1));
                //set_annotation(temp, "boolean");
                $$ = temp;
        }

        | LPAR error RPAR 
        {
                $$ = NULL;
                erro = 1;
                /*printf("ExprError\n");*/
        }

        ;

Aux: Assignment {$$ = $1;}

        | Expr {$$ = $1;}

        ;


%%

void yyerror (char *s){
	// Como STRLIT é um estado do lex o yytext apenas contem a aspa de fechar a string, por isso tem de se usar yylval em vez de yytext
	if (yychar == STRLIT){
		printf("Line %d, col %d: %s: \"%s\"\n", total_lines, (int) (total_columns-strlen(yylval.args->val)) - 2, s, yylval.args->val);
	} else if ((yytext[strlen(yytext) - 1] == '\n') || (yytext[strlen(yytext) - 1] == '\r') || (strcmp(yytext,"\r\n")==0) || yychar == 0){
		printf("Line %d, col %d: %s: %s\n", total_lines, total_columns , s, yytext);
	} else {
		printf("Line %d, col %d: %s: %s\n", total_lines, total_columns-yyleng, s, yytext);
	}
}
