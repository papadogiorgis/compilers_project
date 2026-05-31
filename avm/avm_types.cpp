#include "avm_types.hpp"
#include "../compiler/inc/vm_target_code.h"
#include "avm_stack.hpp"
#include <stdio.h>
#include <cstdlib>
#include <string>
#include "instr.hpp"
#include <cstring>

extern avm_memcell ax, bx, cx, retval;
extern avm_memcell stack[STACK_SZ];
extern unsigned ebp, esp;

avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg){
    switch (arg->type) {
        case global_a: return &stack[STACK_SZ - 1 - arg->val];
        case local_a: return &stack[esp - arg->val];
        case formal_a: return &stack[esp + ENV_SZ + 1 + arg->val];
        case retval_a: return &retval;

        case number_a: {
            reg->type = number_m;
            // reg->data.numVal = consts_getnumber(arg->val);
            return reg;
        }
        case string_a: {
            reg->type = string_m;
            // reg->data.strVal = consts_getstring(arg->val);
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
            reg->type = userfunc_m;
            // reg->data.libFuncVal = libfuncs_getused(arg->val);
            return reg;
        }
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
    }
    else if (lv->type == table_m) {
        // avm_tableincrecounter(lv->data.tableVal);
    }
    return;
}


void avm_table::incrrefcounter(void)
{
    refcnt++;
}

void avm_table::incrrefcounter(void)
{
    refcnt--;
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

