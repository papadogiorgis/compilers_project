/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_SYNTAX_PARSER_H_INCLUDED
# define YY_YY_SYNTAX_PARSER_H_INCLUDED
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
    ASSIGN = 258,
    OR = 259,
    AND = 260,
    EQ = 261,
    NOT_EQ = 262,
    GR_THAN = 263,
    GREQ_THAN = 264,
    LESS_THAN = 265,
    LEQ_THAN = 266,
    PLUS = 267,
    MINUS = 268,
    MULT = 269,
    DIV = 270,
    MOD = 271,
    NOT = 272,
    INCR = 273,
    DECR = 274,
    UMINUS = 275,
    DOT = 276,
    DOTDOT = 277,
    SQ_BR_RIGHT = 278,
    SQ_BR_LEFT = 279,
    LEFT_PAR = 280,
    RIGHT_PAR = 281,
    SEMICOLON = 282,
    COMMA = 283,
    COLON = 284,
    DOUBLE_COLON = 285,
    IF = 286,
    ELSE = 287,
    WHILE = 288,
    FOR = 289,
    FUNC = 290,
    RETURN = 291,
    BREAK = 292,
    LEFT_CURL_BR = 293,
    RIGHT_CURL_BR = 294,
    CONTINUE = 295,
    LOCAL = 296,
    TRUE = 297,
    FALSE = 298,
    NIL = 299,
    ID = 300,
    STRING = 301,
    INT = 302,
    REAL = 303
  };
#endif
/* Tokens.  */
#define ASSIGN 258
#define OR 259
#define AND 260
#define EQ 261
#define NOT_EQ 262
#define GR_THAN 263
#define GREQ_THAN 264
#define LESS_THAN 265
#define LEQ_THAN 266
#define PLUS 267
#define MINUS 268
#define MULT 269
#define DIV 270
#define MOD 271
#define NOT 272
#define INCR 273
#define DECR 274
#define UMINUS 275
#define DOT 276
#define DOTDOT 277
#define SQ_BR_RIGHT 278
#define SQ_BR_LEFT 279
#define LEFT_PAR 280
#define RIGHT_PAR 281
#define SEMICOLON 282
#define COMMA 283
#define COLON 284
#define DOUBLE_COLON 285
#define IF 286
#define ELSE 287
#define WHILE 288
#define FOR 289
#define FUNC 290
#define RETURN 291
#define BREAK 292
#define LEFT_CURL_BR 293
#define RIGHT_CURL_BR 294
#define CONTINUE 295
#define LOCAL 296
#define TRUE 297
#define FALSE 298
#define NIL 299
#define ID 300
#define STRING 301
#define INT 302
#define REAL 303

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 13 "syntax_parser.y"

    int intval;
    char *strval;
    float floatval;


#line 160 "syntax_parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SYNTAX_PARSER_H_INCLUDED  */
