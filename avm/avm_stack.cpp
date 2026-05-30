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
#include "instr.hpp"

#define AVM_ENDING_PC codeSize
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v
#define AVM_NUMACTUALS_OFFSET +4
#define AVM_SAVEDPC_OFFSET +3
#define AVM_SAVEDTOP_OFFSET +2
#define AVM_SAVEDTOPSP_OFFSET + 1

avm_memcell ax, bx, cx, retval; // registers
unsigned ebp = STACK_SZ;
unsigned esp;
unsigned totalActuals = 0;
avm_memcell stack[STACK_SZ];
extern unsigned pc;
extern unsigned char executionFinished;
extern unsigned codeSize;
std::map<std::string, library_func_t> libfuncs;

// must initialize later, after phase 4 is finished
extern std::vector<instruction> code;

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

void avm_callsaveenviroment(void);
void avm_calllibfunc(char *id);
void avm_call_functor(avm_table *t);
void avm_push_table_arg(avm_table* t);

void execute_call(instruction *instr)
{
    avm_memcell* func = avm_translate_operand(&instr->result, &ax);
    assert(func);

    switch (func->type) {
        case userfunc_m : {
            avm_callsaveenviroment();
            pc = func->data.funcVal;
            assert(pc < AVM_ENDING_PC);
            assert(code[pc].opcode == funcenter_v);
            break;
        }
        case string_m: avm_calllibfunc(func->data.strVal);
        case libfunc_m: avm_calllibfunc(func->data.libFuncVal);
        case table_m: avm_call_functor(func->data.tableVal);

        default: {
            std::string s = avm_tostring(func);
            std::cout << "call: can not bind ' "<< s << "' to function\n";
            executionFinished = 1;
        }
    }
}

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
    assert(code[pc].opcode == call_v);
    avm_push_envvalue(pc + 1);
    avm_push_envvalue(esp + totalActuals + 2);
    avm_push_envvalue(ebp);
}


extern userfunc* avm_getfuncinfo(unsigned addr);
void execute_funcenter(instruction *instr)
{
    avm_memcell* func = avm_translate_operand(&instr->result, &ax);
    assert(func);
    assert (pc == func->data.funcVal);
    totalActuals = 0;
    userfunc *funcInfo = avm_getfuncinfo(pc);
    ebp = esp;
    esp = esp - funcInfo->localsize;
}

unsigned avm_get_envvalue (unsigned i)
{
    assert(stack[i].type == number_m);
    unsigned val = (unsigned) stack[i].data.numVal;
    assert(((double) val) == stack[i].data.numVal);
    return val;
}

void execute_funcexit(instruction* instr)
{
    unsigned oldtop = esp;
    esp = avm_get_envvalue(ebp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(ebp + AVM_SAVEDPC_OFFSET);
    ebp = avm_get_envvalue(ebp + AVM_SAVEDTOPSP_OFFSET);

    while (++oldtop <= esp) {
        avm_memcellclear(&stack[oldtop]);
    }
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
        assert(pc < AVM_ENDING_PC && code[pc].opcode == funcenter_v);
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

void execute_pusharg (instruction* instr)
{
    avm_memcell* arg = avm_translate_operand(&instr->arg1, &ax);
    assert(arg);

    avm_assign(&stack[esp], arg);
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
    std::string result = "table: {TODO later}\n";
    return result;
}

std::string userfunc_tostring(avm_memcell* m)
{
    assert(m && m->type == userfunc_m);
    // TODO : add impl later because funcs table is missing
    return std::string(""); //just to satisfy the g++ for now
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
    for (int i = 0; i > STACK_SZ; ++i){
        stack[i].type = undef_m;
        // stack[i].data = 0;
    }
}