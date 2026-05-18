#ifndef INTER_CODE_H
#define INTER_CODE_H

#include "quads.h"
#include <stdio.h>
#include <stdlib.h>

extern int yylineno;

int is_arith(expr* e);
expr* inter_code_assign(expr* lval, expr* rval);
expr* inter_code_arithmetic(expr* lval, expr* rval, iopcode op);
expr *inter_code_bool (expr *lval, expr* rval, iopcode op);
expr* inter_code_objectdef_elist(expr* e);
expr* inter_code_objectdef_indexed(expr* e);
expr* inter_code_member_item(expr* val, expr* index);
expr* inter_code_call(expr* lval, expr* elist);
expr* inter_code_callsuffix(expr* lval, call_struct* callsuffix);
expr* inter_code_uminus(expr* e);
void inter_make_bool_expr(expr* e);
void inter_code_boolean_comparison(expr* lval, expr* arg1, expr* arg2, iopcode op);
expr* inter_code_bool_to_val(expr* e);
expr *inter_code_increment(expr *exp);
expr *inter_code_decrement(expr *exp);
expr *inter_code_decr_var(expr *val);
expr *inter_code_incr_var(expr *val);

#endif