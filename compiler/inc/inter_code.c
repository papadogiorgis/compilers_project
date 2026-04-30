#include "inter_code.h"

int is_arith(expr* e){
    if(e->type == programfunc_e ||
    e->type == libraryfunc_e ||
    e->type == boolexpr_e ||
    e->type == newtable_e ||
    e->type == constbool_e ||
    e->type == conststring_e ||
    e->type == nil_e){
        return 0;
    }
    return 1;
}

expr* arithmetic(expr* lval, expr* rval, iopcode op){
    if(!is_arith(lval) || !is_arith(rval)){
        fprintf(stderr, "ERROR: ILLEGAL ARITHMETIC OPERATION AT LINE %d\n", yylineno);
        return NULL;
    }
    expr* temp_var = newtemp();
    emit(op, lval, rval, temp_var, 0, yylineno);
    return temp_var;
}