#ifndef AVM_DISPATCH
#define AVM_DISPATCH

#include <string>
#include "instr.hpp"

typedef double (*arithmetic_func_t)(double x, double y);
typedef bool (*cmp_func) (double, double);

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

void execute_assign (instruction *);
void execute_add (instruction *);
void execute_sub (instruction *);
void execute_mul (instruction *);
void execute_div (instruction *);
void execute_mod (instruction *);
void execute_uminus (instruction *);
void execute_and (instruction *);
void execute_or (instruction *);
void execute_not (instruction *);
void execute_jeq (instruction *);
void execute_jne (instruction *);
void execute_jle (instruction *);
void execute_jge (instruction *);
void execute_jlt (instruction *);
void execute_jgt (instruction *);
void execute_call (instruction *);
void execute_pusharg (instruction *);
void execute_funcenter (instruction *);
void execute_funcexit (instruction *);
void execute_newtable (instruction *);
void execute_tablegetelem (instruction *);
void execute_tablesetelem (instruction *);
void execute_nop (instruction *);

void execute_ret (instruction *);
void execute_getretval (instruction *);
void execute_jump (instruction *);

void executeCycle (void);
void execute_arithmetic (instruction* i);
void execute_assign(instruction *instr) ;
void execute_newtable(instruction* i);
void execute_tablesetelem(instruction* instr);
void execute_tablegetelem(instruction* instr);
void execute_jeq (instruction* instr);
void execute_nop (instruction* instr);

#endif