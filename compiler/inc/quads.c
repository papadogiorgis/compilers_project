#include "quads.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define EXPAND_SIZE 1024
#define CURR_SIZE (total *sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE*sizeof(quad) + CURR_SIZE)

unsigned currQuad = 1, total = 1;
unsigned int programVarOffset = 0;
unsigned int functionLocalOffset = 0;
unsigned int formalArgOffset = 0;
unsigned int scopeSpaceCounter = 1;
unsigned int tcount = 0;

extern SymTable_T symtable;
extern int yylineno, scope;
quad *quads = NULL;

void expand(void) {
    assert(total == currQuad);
    quad *p = (quad *)malloc(NEW_SIZE);
    if (quads){
        memcpy(p, quads, CURR_SIZE);
        free(quads);
    }
    quads = p;
    total += EXPAND_SIZE;
}

void emit (iopcode op, expr *arg1, expr *arg2, expr *result, unsigned label, unsigned line){
    if (currQuad == total) expand();
    quad *p = quads+currQuad++;
    p->op = op;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;
}

expr *newexpr(expr_t type){
    expr *tmp = malloc(sizeof(expr));
    tmp->type = type;
    return tmp;
}

scopespace_t currscopespace(void){
    if (scopeSpaceCounter == 1) {
        return programvar;
    }
    else if (scopeSpaceCounter % 2 == 0){
        return formalarg;
    }
    else {
        return functionlocal;
    }
}

unsigned int currscopeoffset(void){
    switch(currscopespace()){
        case programvar : return programVarOffset;
        case functionlocal : return functionLocalOffset;
        case formalarg : return formalArgOffset;
        default: assert(0);
    }
}

void incurrscopeoffset (void){
    switch (currscopespace()){
        case programvar : ++programVarOffset; break;
        case functionlocal: ++functionLocalOffset; break;
        case formalarg: ++formalArgOffset; break;
        default: assert(0);
    }
}

void enterscopespace(void){
    ++scopeSpaceCounter;
}

void exitscopespace(void){
    assert(scopeSpaceCounter>1);
    --scopeSpaceCounter;
}

void resetFunctionLocalOffset(void){ functionLocalOffset = 0;}

void resetFormalArgOffset(void) { formalArgOffset = 0;}

expr *newtemp() {
    char *name = malloc(24);
    sprintf(name, "_t%d", tcount++);
    node *sym = SymTable_put(symtable, name, name, LOCALV, scope, yylineno, 0, currscopeoffset());
    // incurrscopeoffset();  ?????
    expr *e = newexpr(var_e);
    e->sym = sym;
    return e;
}

const char *opcode_to_str(iopcode op){
    switch(op) {
        case assign: return "ASSIGN";
        case add: return "ADD";
        case sub: return "SUB";
        default: assert(0);
    }
}


/**
 * Use type of expression to return string for printing quads. 
 */
const char *expr_to_str(expr *e){
    if (!e) return "";
    static char buf[2048];

    switch(e->type){
        case var_e: 
        case arithexpr_e:
        case assignexpr_e: 
            if (e->sym && e->sym->key) return e->sym->key;
            return "anonymous_var";
        case constnum_e: 
            sprintf(buf, "%g", e->numConst);
            return buf;
        case constbool_e: return e->boolConst ? "true" : "false";
        case conststring_e: 
            sprintf(buf, "\"%s\"", e->strConst);
            return buf;
        case nil_e: return "NIL";
        default: return "???"; 
    }
}

void give_quads(FILE* quads_txt){
    fprintf(quads_txt, "quad#   opcode          result          arg1            arg2            label \n");
    fprintf(quads_txt, "------------------------------------------------------------------------------\n");
    for(int i=1; i<currQuad; i++){
        quad *q = &quads[i];

        fprintf(quads_txt, "%-8d ", i);
        fprintf(quads_txt, "%-12s ", opcode_to_str(q->op));
        fprintf(quads_txt, "%-12s ", expr_to_str(q->result));
        fprintf(quads_txt, "%-12s ", expr_to_str(q->arg1));
        fprintf(quads_txt, "%-12s ", expr_to_str(q->arg2));

        if (q->label > 0) {
            fprintf(quads_txt, "%-8d\n", q->label);
        }
        else {
            fprintf(quads_txt, "-\n");
        }
        fprintf(quads_txt, "\n");
    }
}