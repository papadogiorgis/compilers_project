#include "avm_stack.hpp"
#include "avm_types.hpp"
#include <cassert>

#define AVM_ENDING_PC codeSize
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v

unsigned char executionFinished = 0;
unsigned pc = 0;
unsigned currLine = 0;
unsigned codeSize = 0;
instruction *code = 0;

extern void execute_assign (instruction *);
extern void execute_add (instruction *);
extern void execute_sub (instruction *);
extern void execute_mul (instruction *);
extern void execute_div (instruction *);
extern void execute_mod (instruction *);
extern void execute_uminus (instruction *);
extern void execute_and (instruction *);
extern void execute_or (instruction *);
extern void execute_not (instruction *);
extern void execute_jeq (instruction *);
extern void execute_jne (instruction *);
extern void execute_jle (instruction *);
extern void execute_jge (instruction *);
extern void execute_jlt (instruction *);
extern void execute_jgt (instruction *);
extern void execute_call (instruction *);
extern void execute_pusharg (instruction *);
extern void execute_funcenter (instruction *);
extern void execute_funcexit (instruction *);
extern void execute_newtable (instruction *);
extern void execute_tablegetelem (instruction *);
extern void execute_tablesetelem (instruction *);
extern void execute_nop (instruction *);

execute_func_t executeFuncs[] = {
    execute_assign,
    execute_add,
    execute_sub,
    execute_mul,
    execute_div,
    execute_mod,
    execute_uminus,
    execute_and,
    execute_or,
    execute_not,
    execute_jeq,
    execute_jne,
    execute_jle,
    execute_jge,
    execute_jlt,
    execute_jgt,
    execute_call,
    execute_pusharg,
    execute_funcenter,
    execute_funcexit,
    execute_newtable,
    execute_tablegetelem,
    execute_tablesetelem,
    execute_nop
};

void executeCycle (void) {
    if (executionFinished) { return; }
    else if (pc == AVM_ENDING_PC) {
        executionFinished = 1;
        return;
    }
    else {
        assert(pc < AVM_ENDING_PC);
        instruction *instr = code + pc;
        assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
        if (instr->srcline)
            currLine = instr->srcline;
        unsigned oldPc = pc;
        (*executeFuncs[instr->opcode]) (instr);
        if(pc == oldPc) {++pc;}
    }
}

