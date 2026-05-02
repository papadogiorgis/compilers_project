#ifndef INTER_CODE_H
#define INTER_CODE_H

#include "quads.h"
#include <stdio.h>
extern int yylineno;

int is_arith(expr* e);
expr* inter_code_assign(expr* lval, expr* rval);
expr* inter_code_arithmetic(expr* lval, expr* rval, iopcode op);

#endif