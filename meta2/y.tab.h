/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    AND = 258,
    ASSIGN = 259,
    STAR = 260,
    COMMA = 261,
    DIV = 262,
    EQ = 263,
    GE = 264,
    LBRACE = 265,
    LE = 266,
    LPAR = 267,
    LSQ = 268,
    LT = 269,
    MINUS = 270,
    MOD = 271,
    NE = 272,
    NOT = 273,
    OR = 274,
    PLUS = 275,
    RBRACE = 276,
    RPAR = 277,
    RSQ = 278,
    SEMICOLON = 279,
    ARROW = 280,
    LSHIFT = 281,
    RSHIFT = 282,
    XOR = 283,
    BOOL = 284,
    CLASS = 285,
    DOTLENGTH = 286,
    DOUBLE = 287,
    ELSE = 288,
    IF = 289,
    INT = 290,
    PRINT = 291,
    PARSEINT = 292,
    PUBLIC = 293,
    RETURN = 294,
    STATIC = 295,
    STRING = 296,
    VOID = 297,
    WHILE = 298,
    RESERVED = 299,
    INTLIT = 300,
    REALLIT = 301,
    ID = 302,
    STRLIT = 303,
    BOOLLIT = 304,
    GT = 305
  };
#endif
/* Tokens.  */
#define AND 258
#define ASSIGN 259
#define STAR 260
#define COMMA 261
#define DIV 262
#define EQ 263
#define GE 264
#define LBRACE 265
#define LE 266
#define LPAR 267
#define LSQ 268
#define LT 269
#define MINUS 270
#define MOD 271
#define NE 272
#define NOT 273
#define OR 274
#define PLUS 275
#define RBRACE 276
#define RPAR 277
#define RSQ 278
#define SEMICOLON 279
#define ARROW 280
#define LSHIFT 281
#define RSHIFT 282
#define XOR 283
#define BOOL 284
#define CLASS 285
#define DOTLENGTH 286
#define DOUBLE 287
#define ELSE 288
#define IF 289
#define INT 290
#define PRINT 291
#define PARSEINT 292
#define PUBLIC 293
#define RETURN 294
#define STATIC 295
#define STRING 296
#define VOID 297
#define WHILE 298
#define RESERVED 299
#define INTLIT 300
#define REALLIT 301
#define ID 302
#define STRLIT 303
#define BOOLLIT 304
#define GT 305

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 74 "jucompiler.y" /* yacc.c:1909  */

	char* s;
	struct n* no;
	char* t;

#line 160 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
