#include <stdlib.h>

#include "inter_code.h"

extern unsigned int tcount;
extern SymTable_T symtable;
extern int yylineno, scope;

int is_arith(expr* e){
    // if(e->type == programfunc_e ||
    // e->type == libraryfunc_e ||
    // e->type == boolexpr_e ||
    // e->type == newtable_e ||
    // e->type == constbool_e ||
    // e->type == conststring_e ||
    // e->type == nil_e){
    //     return 0;
    // }
    // return 1;
    return (e->type == var_e ||
            e->type == tableitem_e ||
            e->type == arithexpr_e ||
            e->type == assignexpr_e ||
            e->type == constnum_e);
}

expr *emit_if_tableitem(expr *ex){
    if (ex->type != tableitem_e){
        return ex;
    }
    expr *result = newtemp();
    result->type = var_e;
    emit(tablegetelem,ex,  ex->index , result, 0, yylineno);
    return result;
}

expr* inter_code_assign(expr* lval, expr* rval){
    //  TODO: CHECK FOR TABLE ITEMS
    if (lval->type == tableitem_e) {
        emit(tablesetelem, lval->index, rval, lval, 0, yylineno);
        expr *temp = emit_if_tableitem(lval);
        temp->type = assignexpr_e;
        return temp;
    }
    else {
        emit(assign, rval, NULL, lval, 0, yylineno);
        // char *name = malloc(24);
        // sprintf(name, "_t%d", tcount++);
        // node *sym = SymTable_put(symtable, name, name, LOCALV, scope, yylineno, 0, currscopeoffset());
        // // incurrscopeoffset();  ?????
        // expr *e = newexpr(assignexpr_e);
        // e->sym = sym;
        // emit(assign, lval, NULL, e, 0, yylineno);
        return lval;
    }
}

expr* inter_code_arithmetic(expr* lval, expr* rval, iopcode op){
    if(!is_arith(lval) || !is_arith(rval)){
        fprintf(stderr, "ERROR: ILLEGAL ARITHMETIC OPERATION AT LINE %d\n", yylineno);
        return NULL;
    }
    expr* temp_var = newtemp();
    emit(op, lval, rval, temp_var, 0, yylineno);
    return temp_var;
}