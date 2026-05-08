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
    ASSIGN = 258,                  /* ASSIGN  */
    OR = 259,                      /* OR  */
    AND = 260,                     /* AND  */
    EQ = 261,                      /* EQ  */
    NOT_EQ = 262,                  /* NOT_EQ  */
    GR_THAN = 263,                 /* GR_THAN  */
    GREQ_THAN = 264,               /* GREQ_THAN  */
    LESS_THAN = 265,               /* LESS_THAN  */
    LEQ_THAN = 266,                /* LEQ_THAN  */
    PLUS = 267,                    /* PLUS  */
    MINUS = 268,                   /* MINUS  */
    MULT = 269,                    /* MULT  */
    DIV = 270,                     /* DIV  */
    MOD = 271,                     /* MOD  */
    NOT = 272,                     /* NOT  */
    INCR = 273,                    /* INCR  */
    DECR = 274,                    /* DECR  */
    UMINUS = 275,                  /* UMINUS  */
    DOT = 276,                     /* DOT  */
    DOTDOT = 277,                  /* DOTDOT  */
    SQ_BR_RIGHT = 278,             /* SQ_BR_RIGHT  */
    SQ_BR_LEFT = 279,              /* SQ_BR_LEFT  */
    LEFT_PAR = 280,                /* LEFT_PAR  */
    RIGHT_PAR = 281,               /* RIGHT_PAR  */
    SEMICOLON = 282,               /* SEMICOLON  */
    COMMA = 283,                   /* COMMA  */
    COLON = 284,                   /* COLON  */
    DOUBLE_COLON = 285,            /* DOUBLE_COLON  */
    IF = 286,                      /* IF  */
    ELSE = 287,                    /* ELSE  */
    WHILE = 288,                   /* WHILE  */
    FOR = 289,                     /* FOR  */
    FUNC = 290,                    /* FUNC  */
    RETURN = 291,                  /* RETURN  */
    BREAK = 292,                   /* BREAK  */
    LEFT_CURL_BR = 293,            /* LEFT_CURL_BR  */
    RIGHT_CURL_BR = 294,           /* RIGHT_CURL_BR  */
    CONTINUE = 295,                /* CONTINUE  */
    LOCAL = 296,                   /* LOCAL  */
    TRUE = 297,                    /* TRUE  */
    FALSE = 298,                   /* FALSE  */
    NIL = 299,                     /* NIL  */
    ID = 300,                      /* ID  */
    STRING = 301,                  /* STRING  */
    INT = 302,                     /* INT  */
    REAL = 303                     /* REAL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
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
#line 37 "syntax_parser.y"

    int intval;
    char *strval;
    float floatval;
    unsigned uintval;
    struct expr *expression;
    struct node *node;
    struct stmt_t *stmt;
    struct forprefix *forprefix;
    struct call_struct* call_s;

#line 175 "syntax_parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SYNTAX_PARSER_H_INCLUDED  */
