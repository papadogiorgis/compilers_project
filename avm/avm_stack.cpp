#include "avm_stack.hpp"
#include "avm_types.hpp"
#include <cassert>
#include <cstdio>
#include <vector>
#include <cstdlib>
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

// must initialize later, after phase 4 is finished
extern std::vector<instruction> code;

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
        // case string_m: avm_calllibfunc(func->data.strVal);
        // case libfunc_m: avm_calllibfunc(func->data.libFuncVal);
        // case table_m: avm_call_functor(func->data.tableVal);

        default: {
            // char *s = avm_tostring(func);
            // fprintf(stderr, "call: can not bind '%s' to function\n" ,s);
            // free(s);
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
    assert(stack[i].type = number_m);
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