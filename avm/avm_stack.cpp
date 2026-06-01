#include "avm_stack.hpp"
#include "avm_types.hpp"
#include "lib/functions.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <map>
#include <cstring>
#include "instr.hpp"
#include "avm_dispatch.hpp"

avm_memcell ax, bx, cx, retval; // registers
unsigned ebp = STACK_SZ;
unsigned esp;
unsigned totalActuals = 0;
avm_memcell stack[STACK_SZ];
extern unsigned pc;
extern unsigned char executionFinished;
extern unsigned codeSize;
std::map<std::string, library_func_t> libfuncs;

extern instruction* instructions;

tostring_func_t tostringFuncs[]={
    number_tostring,
    string_tostring,
    bool_tostring,
    table_tostring,
    userfunc_tostring,
    libfunc_tostring,
    nil_tostring,
    undef_tostring
};


void avm_dec_top(void)
{
    if (!esp) {
        fprintf(stderr, "stack overflow\n");
        executionFinished = 1;
    }
    else {--esp;}
}

void avm_push_envvalue (unsigned val) 
{
    stack[esp].type = number_m;
    stack[esp].data.numVal = val;
    avm_dec_top();
}

void avm_callsaveenviroment (void) 
{
    avm_push_envvalue(totalActuals);
    assert(instructions[pc].opcode == call_v);
    avm_push_envvalue(pc + 1);
    avm_push_envvalue(esp + totalActuals + 2);
    avm_push_envvalue(ebp);
}


unsigned avm_get_envvalue (unsigned i)
{
    assert(stack[i].type == number_m);
    unsigned val = (unsigned) stack[i].data.numVal;
    assert(((double) val) == stack[i].data.numVal);
    return val;
}

// extern void avm_push_table_arg (avm_table* t);
void avm_call_functor (avm_table *t)
{
    cx.type = string_m;
    cx.data.strVal = strdup("()");
    avm_memcell* f = avm_tablegetelem(t, &cx);
    if (!f) {fprintf(stderr, "in calling table: no '()' element found\n");}
    else if (f->type == table_m) { avm_call_functor(f->data.tableVal); }

    if (f->type == userfunc_m) {
        avm_push_table_arg(t);
        avm_callsaveenviroment();
        pc = f->data.funcVal;
        assert(pc < AVM_ENDING_PC && instructions[pc].opcode == funcenter_v);
    }
    else {
        fprintf(stderr, "in calling table: illegal '()' element value\n");
    }
}

extern library_func_t* lib;
library_func_t avm_getlibfunc(std::string id)
{
    // library_func_t libr = lib[id];
    // return libfuncs.find(id);
    auto iter = libfuncs.find(id);
    if (iter == libfuncs.end()){
        return nullptr;
    }
    return iter->second;
}

void avm_calllibfunc(char *id){
    library_func_t f = avm_getlibfunc(id);
    if (!f) {
        printf("unsupported lib func '%s' called", id);
    }
    else {
        avm_callsaveenviroment();
        ebp = esp;
        totalActuals = 0;
        (*f)();
        if (!executionFinished) {
            execute_funcexit((instruction*)0);
        }
    }
}

unsigned avm_totalactuals (void)
{
    //std::cout << avm_get_envvalue(ebp + AVM_NUMACTUALS_OFFSET) << "\n";
    return avm_get_envvalue(ebp + AVM_NUMACTUALS_OFFSET);
}

avm_memcell* avm_getactual (unsigned i)
{
    assert(i < avm_totalactuals());
    return &stack[ebp + AVM_NUMACTUALS_OFFSET + 1 + i];
}

extern void avm_push_table_arg(avm_table* t)
{
    stack[esp].type = table_m;
    stack[esp].data.tableVal = t;
    stack[esp].data.tableVal->incrrefcounter();
    ++totalActuals;
    avm_dec_top();
}

std::string number_tostring(avm_memcell* m)
{
    assert(m->type == number_m);
    std::string res = std::to_string(m->data.numVal);
    return res;
}

std::string string_tostring(avm_memcell* m)
{
    assert(m && m->type == string_m);
    return m->data.strVal;
}

std::string bool_tostring(avm_memcell* m)
{
    assert(m && m->type == bool_m );
    unsigned char val = m->data.boolVal;
    if (val != 0)
        return "true";
    else 
        return "false";
}

std::string table_tostring(avm_memcell *m)
{
    assert(m && m->type == table_m);
    std::string result = "table: {";
    return result;
}

std::string userfunc_tostring(avm_memcell* m)
{
    assert(m && m->type == userfunc_m);
    // TODO : add impl later because funcs table is missing
    // std::string ret = userFuncs[m->data.funcVal].id;
    // return ret; //just to satisfy the g++ for now

    userfunc* finfo = avm_getfuncinfo_byindex(m->data.funcVal);
    return finfo ? std::string(finfo->id) : "Unknown_Function";
}

std::string libfunc_tostring(avm_memcell* m)
{
    assert(m && m->type == libfunc_m);
    std::string res = m->data.libFuncVal;
    return res;
}

std::string nil_tostring(avm_memcell* m)
{
    assert(m && m->type==nil_m);
    return "nil";
}

std::string undef_tostring(avm_memcell* m)
{
    assert(m && m->type==undef_m);
    return "undef";
}

std::string avm_tostring(avm_memcell* m)
{
    assert(m->type >= 0 && m->type <= undef_m);
    return (tostringFuncs[m->type](m));
}

void avm_register_libfunc(std::string id, library_func_t func)
{
    libfuncs[id] = func;
}

void avm_initfuncs(void) {
    avm_register_libfunc("print", libfunc_print);
    avm_register_libfunc("input", libfunc_input);
    avm_register_libfunc("objectmemberkeys", libfunc_objectmemberkeys);
    avm_register_libfunc("objecttotalmembers", libfunc_objecttotalmembers);
    avm_register_libfunc("objectcopy", libfunc_objectcopy);
    avm_register_libfunc("totalarguments", libfunc_totalarguments);
    avm_register_libfunc("argument", libfunc_argument);
    avm_register_libfunc("typeof", libfunc_typeof);
    avm_register_libfunc("strtonum", libfunc_strtonum);
    avm_register_libfunc("sqrt", libfunc_sqrt);
    avm_register_libfunc("cos", libfunc_cos);
    avm_register_libfunc("sin", libfunc_sin);
}

void avm_initialize_stack(void)
{
    /*STACK_SZ is out of bounds for an array of size STACK_SZ, so i start at STACK_SZ-1*/
    esp = ebp = STACK_SZ - 1;
    ax.type = undef_m;
    bx.type = undef_m;
    cx.type = undef_m;
    for (int i = 0; i < STACK_SZ; ++i){
        AVM_WIPEOUT(stack[i]);
        stack[i].type = undef_m;
    }
}