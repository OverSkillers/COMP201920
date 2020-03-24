/*João Mendes Nº2016230975      Gabriela Costa Nº2017254837*/
%{
    #define MAX_STR 1024
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include "y.tab.h"
    #include "ast.h"

    //If argument -1 is received, set as 1
    int print_tokens;

    //If argument -1 is received, set as 1
    int arvore;

    //If argument -1 is received, set as 1
    int erros_lex;

    //Line and column counter
    int total_lines = 1;
    int total_columns = 1;

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
{ENTER}                                 {  ++total_lines;coluna = total_columns;total_columns=1; };

"/*"                                    {  error_line_number_Comment = total_lines; error_line_column_Comment = total_columns;coluna = total_columns; total_columns+= 2;comment = 1;BEGIN COMMENT;};
<COMMENT>{ENTER}                           ++total_lines; total_columns=1;
<COMMENT>[^["*/"]]*                        total_columns += strlen(yytext);
<COMMENT>"*/"                           {  total_columns += 2; comment = 0; BEGIN 0;};
<COMMENT>.                              {  total_columns += strlen(yytext);};

"//"                                    {  BEGIN COMMENT2;};
<COMMENT2>{ENTER}                       {  ++total_lines; total_columns=1;BEGIN 0;}
<COMMENT2>.                             ;

\"                                      {  error_line_column_String = total_columns;coluna = total_columns;  ++total_columns; current_string[0] = '\0';erro_string=0;BEGIN STRING;}
<STRING>([^\"\n\r\\]|\\\\|\\[fnrt\\\"])+ {strcat(current_string, yytext);total_columns += strlen(yytext);};
<STRING>(\\[^fnrt\n\r\\\"]|\\)          {if(erros_lex == 1){printf("Line %d, col %d: invalid escape sequence (%s)\n", total_lines, total_columns, yytext);}erro_string=1;total_columns += strlen(yytext);};
<STRING>{ENTER}                         {BEGIN 0;if(erros_lex == 1){printf("Line %d, col %d: unterminated string literal\n", total_lines, error_line_column_String);};total_columns=1;++total_lines;erro_string=1;};
<STRING>\"                              {BEGIN 0;++total_columns;if(erro_string==0)
                                            {if (print_tokens==1){printf("STRLIT(\"%s\")\n", current_string);}if(arvore == 1) {yylval.s = current_string;return STRLIT;}}};

"true"|"false"                          {  total_columns += strlen(yytext); if (print_tokens==1) printf("BOOLLIT(%s)\n",yytext); if(arvore == 1) {yylval.s = strdup(yytext);return BOOLLIT;};};
"&&"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("AND\n");if(arvore == 1) return AND;};
"="                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("ASSIGN\n");if(arvore == 1) return ASSIGN;};
"*"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("STAR\n");if(arvore == 1) return STAR;};
","                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("COMMA\n");if(arvore == 1) return COMMA;};
"/"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("DIV\n");if(arvore == 1) return DIV;};
"=="                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("EQ\n");if(arvore == 1) return EQ;};
">="                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("GE\n");if(arvore == 1) return GE;};
">"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("GT\n");if(arvore == 1) return GT;};
"{"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("LBRACE\n");if(arvore == 1) return LBRACE;};
"<="                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("LE\n");if(arvore == 1) return LE;};
"("                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("LPAR\n");if(arvore == 1) return LPAR;};
"["                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("LSQ\n");if(arvore == 1) return LSQ;};
"<"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("LT\n");if(arvore == 1) return LT;};
"-"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("MINUS\n");if(arvore == 1) return MINUS;};
"%"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("MOD\n");if(arvore == 1) return MOD;};
"!="                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("NE\n");if(arvore == 1) return NE;};
"!"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("NOT\n");if(arvore == 1) return NOT;};
"||"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("OR\n");if(arvore == 1) return OR;};
"+"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("PLUS\n");if(arvore == 1) return PLUS;};
"}"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("RBRACE\n");if(arvore == 1) return RBRACE;};
")"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("RPAR\n");if(arvore == 1) return RPAR;};
"]"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("RSQ\n");if(arvore == 1) return RSQ;};
";"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("SEMICOLON\n");if(arvore == 1) return SEMICOLON;};
"->"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("ARROW\n");if(arvore == 1) return ARROW;};
"<<"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("LSHIFT\n");if(arvore == 1) return LSHIFT;};
">>"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("RSHIFT\n");if(arvore == 1) return RSHIFT;};
"^"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("XOR\n");if(arvore == 1) return XOR;};
"boolean"                               {  total_columns += strlen(yytext); if (print_tokens==1) printf("BOOL\n");if(arvore == 1) return BOOL;};
"class"                                 {  total_columns += strlen(yytext); if (print_tokens==1) printf("CLASS\n");if(arvore == 1) return CLASS;};
".length"                               {  total_columns += strlen(yytext); if (print_tokens==1) printf("DOTLENGTH\n");if(arvore == 1) return DOTLENGTH;};
"double"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("DOUBLE\n");if(arvore == 1) return DOUBLE;};
"else"                                  {  total_columns += strlen(yytext); if (print_tokens==1) printf("ELSE\n");if(arvore == 1) return ELSE;};
"if"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("IF\n");if(arvore == 1) return IF;};
"int"                                   {  total_columns += strlen(yytext); if (print_tokens==1) printf("INT\n");if(arvore == 1) return INT;};
"System.out.print"                      {  total_columns += strlen(yytext); if (print_tokens==1) printf("PRINT\n");if(arvore == 1) return PRINT;};
"Integer.parseInt"                      {  total_columns += strlen(yytext); if (print_tokens==1) printf("PARSEINT\n");if(arvore == 1) return PARSEINT;};
"public"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("PUBLIC\n");if(arvore == 1) return PUBLIC;};
"return"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("RETURN\n");if(arvore == 1) return RETURN;};
"static"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("STATIC\n");if(arvore == 1) return STATIC;};
"String"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("STRING\n");if(arvore == 1) return STR;};
"void"                                  {  total_columns += strlen(yytext); if (print_tokens==1) printf("VOID\n");if(arvore == 1) return VOID;};
"while"                                 {  total_columns += strlen(yytext); if (print_tokens==1) printf("WHILE\n");if(arvore == 1) return WHILE;};

{RESERVED}                              {  total_columns += strlen(yytext); if (print_tokens==1) printf("RESERVED(%s)\n", yytext);if(arvore == 1) {yylval.s = strdup(yytext);return RESERVED;}};
{ID}                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("ID(%s)\n", yytext);if(arvore == 1) {yylval.s = strdup(yytext);return ID;}};
{INT}                                   {  total_columns += strlen(yytext); if (print_tokens==1) printf("INTLIT(%s)\n", yytext);if(arvore == 1) {yylval.s = strdup(yytext);return INTLIT;}};
{REALLIT}                               {  total_columns += strlen(yytext); if (print_tokens==1) printf("REALLIT(%s)\n", yytext);if(arvore == 1) {yylval.s = strdup(yytext);return REALLIT;}};

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

            if(comment == 1 && erros_lex == 1){
                printf("Line %d, col %d: unterminated comment\n",error_line_number_Comment, error_line_column_Comment);
            }
        }
        if (strcmp(argv[1], "-e1")==0){
            print_tokens = 0;
            arvore = 0;
            erros_lex = 1;
            yylex();

            if(comment == 1 && erros_lex == 1){
                printf("Line %d, col %d: unterminated comment\n",error_line_number_Comment, error_line_column_Comment);
            }
        }
        if (strcmp(argv[1], "-t")==0){
            print_tokens = 0;
            arvore = 1;
            erros_lex = 0;
            yyparse();}
        if (strcmp(argv[1], "-e2")==0){
            print_tokens = 0;
            arvore = 1;
            erros_lex = 0;
            yyparse();}
    }
    else{
        print_tokens = 0;
        arvore = 1;
        erros_lex = 1;
        yylex();
        if(comment == 1 && erros_lex == 1){
                printf("Line %d, col %d: unterminated comment\n",error_line_number_Comment, error_line_column_Comment);
            }
        yyparse();
        }


    return 0;
}

int yywrap()
{
    return 1;
}
