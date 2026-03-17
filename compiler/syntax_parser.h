/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SYNTAX_PARSER_H_INCLUDED
# define YY_YY_SYNTAX_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    INTEGER = 259,                 /* INTEGER  */
    INT = 260,                     /* INT  */
    REAL = 261,                    /* REAL  */
    ASSIGN = 262,                  /* ASSIGN  */
    OR = 263,                      /* OR  */
    AND = 264,                     /* AND  */
    EQ = 265,                      /* EQ  */
    NOT_EQ = 266,                  /* NOT_EQ  */
    GR_THAN = 267,                 /* GR_THAN  */
    GREQ_THAN = 268,               /* GREQ_THAN  */
    LESS_THAN = 269,               /* LESS_THAN  */
    LEQ_THAN = 270,                /* LEQ_THAN  */
    PLUS = 271,                    /* PLUS  */
    MINUS = 272,                   /* MINUS  */
    MULT = 273,                    /* MULT  */
    DIV = 274,                     /* DIV  */
    MOD = 275,                     /* MOD  */
    NOT = 276,                     /* NOT  */
    INCR = 277,                    /* INCR  */
    DECR = 278,                    /* DECR  */
    UMINUS = 279,                  /* UMINUS  */
    DOT = 280,                     /* DOT  */
    DOTDOT = 281,                  /* DOTDOT  */
    SQ_BR_RIGHT = 282,             /* SQ_BR_RIGHT  */
    SQ_BR_LEFT = 283,              /* SQ_BR_LEFT  */
    LEFT_PAR = 284,                /* LEFT_PAR  */
    RIGHT_PAR = 285,               /* RIGHT_PAR  */
    SEMICOLON = 286,               /* SEMICOLON  */
    COMMA = 287,                   /* COMMA  */
    COLON = 288,                   /* COLON  */
    DOUBLE_COLON = 289,            /* DOUBLE_COLON  */
    IF = 290,                      /* IF  */
    ELSE = 291,                    /* ELSE  */
    WHILE = 292,                   /* WHILE  */
    FOR = 293,                     /* FOR  */
    FUNC = 294,                    /* FUNC  */
    RETURN = 295,                  /* RETURN  */
    BREAK = 296,                   /* BREAK  */
    LEFT_CURL_BR = 297,            /* LEFT_CURL_BR  */
    RIGHT_CURL_BR = 298,           /* RIGHT_CURL_BR  */
    CONTINUE = 299,                /* CONTINUE  */
    LOCAL = 300,                   /* LOCAL  */
    TRUE = 301,                    /* TRUE  */
    FALSE = 302,                   /* FALSE  */
    NIL = 303                      /* NIL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define ID 258
#define INTEGER 259
#define INT 260
#define REAL 261
#define ASSIGN 262
#define OR 263
#define AND 264
#define EQ 265
#define NOT_EQ 266
#define GR_THAN 267
#define GREQ_THAN 268
#define LESS_THAN 269
#define LEQ_THAN 270
#define PLUS 271
#define MINUS 272
#define MULT 273
#define DIV 274
#define MOD 275
#define NOT 276
#define INCR 277
#define DECR 278
#define UMINUS 279
#define DOT 280
#define DOTDOT 281
#define SQ_BR_RIGHT 282
#define SQ_BR_LEFT 283
#define LEFT_PAR 284
#define RIGHT_PAR 285
#define SEMICOLON 286
#define COMMA 287
#define COLON 288
#define DOUBLE_COLON 289
#define IF 290
#define ELSE 291
#define WHILE 292
#define FOR 293
#define FUNC 294
#define RETURN 295
#define BREAK 296
#define LEFT_CURL_BR 297
#define RIGHT_CURL_BR 298
#define CONTINUE 299
#define LOCAL 300
#define TRUE 301
#define FALSE 302
#define NIL 303

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 13 "syntax_parser.y"

    int intval;
    char *strval;
    float floatval;


#line 170 "syntax_parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SYNTAX_PARSER_H_INCLUDED  */
