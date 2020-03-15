%{
    #define MAX_STR 1024

    //Joao Mendes 2016230975
    //Gabriela Costa 2017254837
    //If argument -1 is received, set as 1
    int print_tokens;

    //Line and column counter
    int total_lines = 1;
    int total_columns = 1;

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

%X COMMENT
%X COMMENT2
%X STRING

%%

[ \t]+                                     total_columns += strlen(yytext);
[\f]+                                      total_columns += strlen(yytext);
{ENTER}                                 {  ++total_lines; total_columns=1; };

"/*"                                    {  error_line_number_Comment = total_lines; error_line_column_Comment = total_columns; total_columns+= 2;comment = 1;BEGIN COMMENT;};
<COMMENT>{ENTER}                           ++total_lines; total_columns=1;
<COMMENT>[^["*/"]]*                        total_columns += strlen(yytext);
<COMMENT>"*/"                           {  total_columns += 2; comment = 0; BEGIN 0;};
<COMMENT>.                              {  total_columns += strlen(yytext);};

"//"                                    {  BEGIN COMMENT2;};
<COMMENT2>{ENTER}                       {  ++total_lines; total_columns=1;BEGIN 0;}
<COMMENT2>.                             ;

\"                                      {  error_line_column_String = total_columns;  ++total_columns; current_string[0] = '\0';erro_string=0;BEGIN STRING;}
<STRING>([^\"\n\r\\]|\\\\|\\[fnrt\\\"])+ {strcat(current_string, yytext);total_columns += strlen(yytext);};
<STRING>(\\[^fnrt\n\r\\\"]|\\)          {printf("Line %d, col %d: invalid escape sequence (%s)\n", total_lines, total_columns, yytext);erro_string=1;total_columns += strlen(yytext);};
<STRING>{ENTER}                         {BEGIN 0;printf("Line %d, col %d: unterminated string literal\n", total_lines, error_line_column_String);total_columns=1;++total_lines;erro_string=1;};
<STRING>\"                              {BEGIN 0;++total_columns;if(erro_string==0)
                                            {if (print_tokens==1){printf("STRLIT(\"%s\")\n", current_string);}}};

"true"|"false"                          {  total_columns += strlen(yytext); if (print_tokens==1) printf("BOOLLIT(%s)\n",yytext);};
"&&"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("AND\n");};
"="                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("ASSIGN\n");};
"*"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("STAR\n");};
","                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("COMMA\n");};
"/"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("DIV\n");};
"=="                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("EQ\n");};
">="                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("GE\n");};
">"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("GT\n");};
"{"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("LBRACE\n");};
"<="                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("LE\n");};
"("                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("LPAR\n");};
"["                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("LSQ\n");};
"<"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("LT\n");};
"-"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("MINUS\n");};
"%"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("MOD\n");};
"!="                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("NE\n");};
"!"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("NOT\n");};
"||"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("OR\n");};
"+"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("PLUS\n");};
"}"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("RBRACE\n");};
")"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("RPAR\n");};
"]"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("RSQ\n");};
";"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("SEMICOLON\n");};
"->"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("ARROW\n");};
"<<"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("LSHIFT\n");};
">>"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("RSHIFT\n");};
"^"                                     {  total_columns += strlen(yytext); if (print_tokens==1) printf("XOR\n");};
"boolean"                               {  total_columns += strlen(yytext); if (print_tokens==1) printf("BOOL\n");};
"class"                                 {  total_columns += strlen(yytext); if (print_tokens==1) printf("CLASS\n");};
".length"                               {  total_columns += strlen(yytext); if (print_tokens==1) printf("DOTLENGTH\n");};
"double"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("DOUBLE\n");};
"else"                                  {  total_columns += strlen(yytext); if (print_tokens==1) printf("ELSE\n");};
"if"                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("IF\n");};
"int"                                   {  total_columns += strlen(yytext); if (print_tokens==1) printf("INT\n");};
"System.out.print"                      {  total_columns += strlen(yytext); if (print_tokens==1) printf("PRINT\n");};
"Integer.parseInt"                      {  total_columns += strlen(yytext); if (print_tokens==1) printf("PARSEINT\n");};
"public"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("PUBLIC\n");};
"return"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("RETURN\n");};
"static"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("STATIC\n");};
"String"                                {  total_columns += strlen(yytext); if (print_tokens==1) printf("STRING\n");};
"void"                                  {  total_columns += strlen(yytext); if (print_tokens==1) printf("VOID\n");};
"while"                                 {  total_columns += strlen(yytext); if (print_tokens==1) printf("WHILE\n");};

{RESERVED}                              {  total_columns += strlen(yytext); if (print_tokens==1) printf("RESERVED(%s)\n", yytext);};
{ID}                                    {  total_columns += strlen(yytext); if (print_tokens==1) printf("ID(%s)\n", yytext);};
{INT}                                   {  total_columns += strlen(yytext); if (print_tokens==1) printf("INTLIT(%s)\n", yytext);};
{REALLIT}                               {  total_columns += strlen(yytext); if (print_tokens==1) printf("REALLIT(%s)\n", yytext);};

.                                       {printf("Line %d, col %d: illegal character (%s)\n", total_lines, total_columns, yytext); total_columns += strlen(yytext);};


%%
int main(int argc, char **argv)
{
    if (argc > 1){
        if (strcmp(argv[1], "-l")==0) print_tokens = 1;
        if (strcmp(argv[1], "-e1")==0) print_tokens = 0;
    }
    else print_tokens = 0;

    yylex();

    if(comment == 1){
        printf("Line %d, col %d: unterminated comment\n", error_line_number_Comment, error_line_column_Comment);
    }

    return 0;
}

int yywrap()
{
    return 1;
}
