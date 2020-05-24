/*João Mendes Nº2016230975      Gabriela Costa Nº2017254837*/
%{
    #define MAX_STR 1024
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include "y.tab.h"
    #include "ast.h"
    #include "semantics.h"
    #include "symbol.h"

    //If argument -1 is received, set as 1
    int print_tokens;

    //If argument -1 is received, set as 1
    int arvore;

    //If argument -1 is received, set as 1
    int erros_lex;

    //Line and column counter
    int total_lines = 1;
    int total_columns = 1;

    int linha = 1;
    int coluna = 1;

    int erro_string = 0;


    //Save first line and column where a possible error might occur
    int error_line_number_String;
    int error_line_column_String;
    int error_line_number_Comment;
    int error_line_column_Comment;

    // /*...*/ like comments. to detect if analysis ended and it wasnt closed
    int comment =  0;

    //Hold current strlit
    char current_string[MAX_STR];

    //Hold current package ID
    char current_package_id[MAX_STR];

    extern node* tree;
    extern int erro;
    extern table_t* table;

    void init_yylval(int line, int col, char* text);
%}
ENTER                                   \n|\r|\r\n
ID                                      [A-Za-z_$][0-9A-Za-z_$]*
RESERVED                                (abstract|continue|for|new|switch|assert|default|package|synchronized|do|goto|private|this|break|implements|protected|throw|byte|import|throws|case|enum|instanceof|transient|catch|extends|short|try|char|final|interface|finally|long|strictfp|volatile|const|float|native|super|"++"|"--"|null|Integer|System)
INT                                     ([0-9]|([1-9]+[0-9_]*[0-9])+)
EXPOENTE                                ([eE])("+"|"-")?([0-9]|([0-9]+[0-9_]*[0-9])+)
NUM                                     [0-9]+|([0-9]+[0-9_]*[0-9])+
REALLIT                                 {NUM}"."{NUM}?{EXPOENTE}?)|("."{NUM}{EXPOENTE}?)|({NUM}{EXPOENTE}

%option yylineno
%X COMMENT
%X COMMENT2
%X STRING

%%

[ \t]+                                     total_columns += strlen(yytext);
[\f]+                                      total_columns += strlen(yytext);
{ENTER}                                 {  linha = total_lines;++total_lines;coluna = total_columns;total_columns=1; };

"/*"                                    {  error_line_number_Comment = total_lines; error_line_column_Comment = total_columns;coluna = total_columns; total_columns+= 2;comment = 1;BEGIN COMMENT;};
<COMMENT>{ENTER}                           ++total_lines; total_columns=1;
<COMMENT>[^["*/"]]*                        total_columns += strlen(yytext);
<COMMENT>"*/"                           {  total_columns += 2; comment = 0; BEGIN 0;};
<COMMENT>.                              {  total_columns += strlen(yytext);};
<COMMENT><<EOF>>                        {  if(erros_lex == 1) printf("Line %d, col %d: unterminated comment\n",error_line_number_Comment, error_line_column_Comment); BEGIN 0;}

"//"                                    {  coluna = total_columns;BEGIN COMMENT2;};
<COMMENT2>{ENTER}                       {  ++total_lines; total_columns=1;BEGIN 0;}
<COMMENT2>.                             ;

\"                                      {  error_line_column_String = total_columns;coluna = total_columns;  ++total_columns; current_string[0] = '\0';erro_string=0;BEGIN STRING;}
<STRING>([^\"\n\r\\]|\\\\|\\[fnrt\\\"])+ {strcat(current_string, yytext);total_columns += strlen(yytext);};
<STRING>(\\[^fnrt\n\r\\\"]|\\)          {if(erros_lex == 1){printf("Line %d, col %d: invalid escape sequence (%s)\n", total_lines, total_columns, yytext);}erro_string=1;total_columns += strlen(yytext);};
<STRING>{ENTER}                         {BEGIN 0;if(erros_lex == 1){printf("Line %d, col %d: unterminated string literal\n", total_lines, error_line_column_String);};total_columns=1;++total_lines;erro_string=1;};
<STRING>\"                              {BEGIN 0;++total_columns;if(erro_string==0)
                                            {if (print_tokens==1){printf("STRLIT(\"%s\")\n", current_string);}if(arvore == 1) {init_yylval(total_lines, error_line_column_String, current_string);return STRLIT;}}};

"true"|"false"                          {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("BOOLLIT(%s)\n",yytext); if(arvore == 1) {init_yylval(total_lines, error_line_column_String, strdup(yytext));return BOOLLIT;};};
"&&"                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("AND\n");if(arvore == 1) return AND;};
"="                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("ASSIGN\n");if(arvore == 1) return ASSIGN;};
"*"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("STAR\n");if(arvore == 1) return STAR;};
","                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("COMMA\n");if(arvore == 1) return COMMA;};
"/"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("DIV\n");if(arvore == 1) return DIV;};
"=="                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("EQ\n");if(arvore == 1) return EQ;};
">="                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("GE\n");if(arvore == 1) return GE;};
">"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("GT\n");if(arvore == 1) return GT;};
"{"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("LBRACE\n");if(arvore == 1) return LBRACE;};
"<="                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("LE\n");if(arvore == 1) return LE;};
"("                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("LPAR\n");if(arvore == 1) return LPAR;};
"["                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("LSQ\n");if(arvore == 1) return LSQ;};
"<"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("LT\n");if(arvore == 1) return LT;};
"-"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("MINUS\n");if(arvore == 1) return MINUS;};
"%"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("MOD\n");if(arvore == 1) return MOD;};
"!="                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("NE\n");if(arvore == 1) return NE;};
"!"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("NOT\n");if(arvore == 1) return NOT;};
"||"                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("OR\n");if(arvore == 1) return OR;};
"+"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("PLUS\n");if(arvore == 1) return PLUS;};
"}"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("RBRACE\n");if(arvore == 1) return RBRACE;};
")"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("RPAR\n");if(arvore == 1) return RPAR;};
"]"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("RSQ\n");if(arvore == 1) return RSQ;};
";"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("SEMICOLON\n");if(arvore == 1) return SEMICOLON;};
"->"                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("ARROW\n");if(arvore == 1) return ARROW;};
"<<"                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("LSHIFT\n");if(arvore == 1) return LSHIFT;};
">>"                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("RSHIFT\n");if(arvore == 1) return RSHIFT;};
"^"                                     {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("XOR\n");if(arvore == 1) return XOR;};
"boolean"                               {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("BOOL\n");if(arvore == 1) return BOOL;};
"class"                                 {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("CLASS\n");if(arvore == 1) return CLASS;};
".length"                               {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("DOTLENGTH\n");if(arvore == 1) return DOTLENGTH;};
"double"                                {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("DOUBLE\n");if(arvore == 1) return DOUBLE;};
"else"                                  {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("ELSE\n");if(arvore == 1) return ELSE;};
"if"                                    {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("IF\n");if(arvore == 1) return IF;};
"int"                                   {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("INT\n");if(arvore == 1) return INT;};
"System.out.print"                      {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("PRINT\n");if(arvore == 1) return PRINT;};
"Integer.parseInt"                      {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("PARSEINT\n");if(arvore == 1) return PARSEINT;};
"public"                                {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("PUBLIC\n");if(arvore == 1) return PUBLIC;};
"return"                                {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("RETURN\n");if(arvore == 1) return RETURN;};
"static"                                {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("STATIC\n");if(arvore == 1) return STATIC;};
"String"                                {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("STRING\n");if(arvore == 1) return STR;};
"void"                                  {  init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("VOID\n");if(arvore == 1) return VOID;};
"while"                                 { init_yylval(total_lines,
total_columns,NULL);total_columns += strlen(yytext); if (print_tokens==1) printf("WHILE\n");if(arvore == 1) return WHILE;};

{RESERVED}                              {  total_columns += strlen(yytext); if (print_tokens==1) printf("RESERVED(%s)\n", yytext);if(arvore == 1) {init_yylval(total_lines, total_columns, strdup(yytext));return RESERVED;}};
{ID}                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("ID(%s)\n", yytext);if(arvore == 1) {init_yylval(total_lines, total_columns, strdup(yytext));return ID;}};
{INT}                                   {  total_columns += strlen(yytext); if (print_tokens==1) printf("INTLIT(%s)\n", yytext);if(arvore == 1) {init_yylval(total_lines, total_columns, strdup(yytext));return INTLIT;}};
{REALLIT}                               {  total_columns += strlen(yytext); if (print_tokens==1) printf("REALLIT(%s)\n", yytext);if(arvore == 1) {init_yylval(total_lines, total_columns, strdup(yytext));return REALLIT;}};

.                                       {if(erros_lex == 1){printf("Line %d, col %d: illegal character (%s)\n", total_lines, total_columns, yytext);} total_columns += strlen(yytext);};


%%
int main(int argc, char **argv)
{
    if (argc > 1){
        if (strcmp(argv[1], "-l")==0){
            print_tokens = 1;
            erros_lex = 1;
            arvore = 0;
            yylex();
        }
        if (strcmp(argv[1], "-e1")==0){
            print_tokens = 0;
            arvore = 0;
            erros_lex = 1;
            yylex();
        }
        if (strcmp(argv[1], "-t")==0){
            print_tokens = 0;
            arvore = 1;
            erros_lex = 1;
            yyparse();
            if(erro == 0){
                if (arvore == 1){
                    print_tree(0,tree,0);
                    free_tree(tree);
                }
            }
        }
        if (strcmp(argv[1], "-s")==0){
            print_tokens = 0;
            arvore = 1;
            erros_lex = 1;
            yyparse();
            if(erro == 0){
                if (arvore == 1){
                    table_t* tables = sem(tree, tree->son);
                    print_table(tables);
                    print_tree(0,tree,1);
                    free_tree(tree);
                }
            }
        }
        if (strcmp(argv[1], "-e2")==0){
            print_tokens = 0;
            arvore = 1;
            erros_lex = 1;
            yyparse();
            yylex();
        }
    }
    else{
        print_tokens = 0;
        arvore = 1;
        erros_lex = 0;
        yyparse();
        if(erro == 0){
                sem(tree, tree->son);
                free_tree(tree);
            }
        }


    return 0;
}

void init_yylval(int line, int col, char* text){
	yylval.args = (struct _args *) malloc(sizeof(struct _args));
	yylval.args->line = line;
	yylval.args->col = col;
	if (text != NULL){
		yylval.args->val = strdup(text);
	} else {
		yylval.args->val = NULL;
	}
}

int yywrap()
{
    return 1;
}
