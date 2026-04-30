#ifndef INTER_CODE_H
#define INTER_CODE_H

#include "quads.h"
#include <stdio.h>
extern int yylineno;

int is_arith(expr* e);
expr* arithmetic(expr* lval, expr* rval, iopcode op);

#endif