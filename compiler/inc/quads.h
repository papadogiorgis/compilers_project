#ifndef QUADS_H
#define QUADS_H

#include "symtable.h"

typedef enum expr_t {
    var_e,
    tableitem_e,
    programfunc_e,
    libraryfunc_e,
    arithexpr_e,
    boolexpr_e,
    assignexpr_e,
    newtable_e,
    constnum_e,
    constbool_e,
    conststring_e,

    nil_e,
}expr_t;

typedef struct expr {
    expr_t type;
    node *sym; // symbol
    struct expr *index;
    double numConst;
    char *strConst;
    unsigned char boolConst;
    struct expr *next;
} expr;

typedef enum iopcode {
    assign, add, sub, mul, division, mod,
    uminus, and_op, or_op, not_op, if_eq, if_noteq,
    if_lesseq, if_greatereq, if_less, if_greater, 
    call, param, ret, getretval, funcstart, funcend,
    tablecreate, tablegetelem, tablesetelem
} iopcode;

typedef struct quad {
    iopcode op;
    expr *result;
    expr *arg1;
    expr *arg2;
    unsigned int label;
    unsigned int line;
}quad;

typedef enum scopespace_t {
    programvar,
    functionlocal,
    formalarg
}scopespace_t;

enum symbol_t {var_s, programfunc_s, libraryfunc_s};



struct expr *newexpr(expr_t type);

void emit(iopcode op, expr *arg1, expr *arg2, expr *result, unsigned label, unsigned line);

expr *newtemp();

void resetFunctionLocalOffset(void);

void resetFormalArgOffset(void);

void enterscopespace();

void exitscopespace(void);

scopespace_t currscopespace(void);

unsigned int currscopeoffset(void);

void incurrscopeoffset (void);

#endif