#include "avm_types.hpp"
#include "avm_stack.hpp"
#include "instr.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

extern avm_memcell ax, bx, cx, retval;
extern avm_memcell stack[STACK_SZ];
extern unsigned ebp, esp;
extern double* numConsts;
extern char** stringConsts;
extern char** namedLibfuncs;
extern userfunc* userFuncs;
extern unsigned totalUserFuncs;

const char* typeStrings[] = {
    "number",
    "string",
    "bool",
    "table",
    "userfunc",
    "libfunc",
    "nil",
    "undef"
};

unsigned totalTables=0;

avm_table::avm_table(){
    refcnt=0;
    tableno= ++totalTables;
}

avm_table::~avm_table(){
    for(auto& pair : numIndexed){
        avm_memcellclear(pair.second);
        delete pair.second;
    }
    for(auto& pair : strIndexed){
        avm_memcellclear(pair.second);
        delete pair.second;
    }
    for(auto& pair : otherIndexed){
        avm_memcellclear(pair.second);
        delete pair.second;
    }
    numIndexed.clear();
    strIndexed.clear();
    otherIndexed.clear();
}

void avm_table::incrrefcounter(void){
    ++refcnt;
}

void avm_table::decrrefcounter(void){
    assert(refcnt>0);
    if(--refcnt == 0){
        delete this;
    }
}

avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index){
    if(index->type == number_m){
        auto temp = table->numIndexed.find(index->data.numVal);
        if(temp != table->numIndexed.end()){
            return temp->second;
        }
    }else if(index->type == string_m){
        auto temp = table->strIndexed.find(index->data.strVal);
        if(temp != table->strIndexed.end()){
            return temp->second;
        }
    }else{
        auto temp = table->otherIndexed.find(index);
        if(temp != table->otherIndexed.end()){
            return temp->second;
        }
    }
    return nullptr;
}

userfunc* avm_getfuncinfo(unsigned address){
    for(unsigned i=0; i<totalUserFuncs; ++i){
        if(userFuncs[i].address == address){
            return &userFuncs[i];
        }
    }
    return nullptr;
}

avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg){
    switch (arg->type) {
        case global_a: return &stack[STACK_SZ - 1 - arg->val];
        case local_a: return &stack[esp - arg->val];
        case formal_a: return &stack[esp + ENV_SZ + 1 + arg->val];
        case retval_a: return &retval;

        case number_a: {
            reg->type = number_m;
            reg->data.numVal = numConsts[arg->val];
            return reg;
        }
        case string_a: {
            reg->type = string_m;
            reg->data.strVal = strdup(stringConsts[arg->val]);
            return reg;
        }
        case bool_a: {
            reg->type = bool_m;
            reg->data.boolVal = arg->val;
            return reg;
        }
        case nil_a: reg->type = nil_m; return reg;
        case userfunc_a: {
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;
        }
        case libfunc_a: {
            reg->type = libfunc_m;
            reg->data.libFuncVal = strdup(namedLibfuncs[arg->val]);
            return reg;
        }
        default:
            return nullptr;
    }
}

void avm_assign (avm_memcell *lv, avm_memcell *rv)
{
    if (lv == rv) {return;}

    // same tables
    if (lv->type == table_m && rv->type == table_m && lv->data.tableVal == rv->data.tableVal){
        return;
    }

    if (rv->type == undef_m) {
        fprintf(stderr, "warning: assigning from undef content\n");
    }

    avm_memcellclear(lv);
    memcpy(lv, rv, sizeof(avm_memcell));

    if (lv->type == string_m){
        lv->data.strVal = strdup(rv->data.strVal);
    }else if (lv->type == libfunc_m) {
        lv->data.libFuncVal = strdup(rv->data.libFuncVal);
    }else if (lv->type == table_m) {
        lv->data.tableVal->incrrefcounter();
    }
    return;
}