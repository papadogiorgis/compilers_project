#include "vm_target_code.h"

void generate_loop(void){}

void make_operand(expr* e, vmarg* arg){}

void add_incomplete_jump(unsigned instrNo, unsigned iaddress);
void patch_incomplete_jumps(void);
unsigned consts_newstring(char* s);
unsigned consts_newnumber(double n);
unsigned libfuncs_nevused(char* s);
unsigned userfuncs_newfunc(node* sym);
void generate_ASSIGN(quad* q);
void generate_ADD(quad* q);
void generate_SUB(quad* q);
void generate_MUL(quad* q);
void generate_DIV(quad* q);
void generate_MOD(quad* q);
void generate_UMINUS(quad* q);
void generate_AND(quad* q);
void generate_OR(quad* q);
void generate_NOT(quad* q);
void generate_JEQ(quad* q);
void generate_JNE(quad* q);
void generate_JLE(quad* q);
void generate_JGE(quad* q);
void generate_JLT(quad* q);
void generate_JGT(quad* q);
void generate_CALL(quad* q);
void generate_PUSHARG(quad* q);
void generate_FUNCENTER(quad* q);
void generate_FUNCEXIT(quad* q);
void generate_NEWTABLE(quad* q);
void generate_TABLEGETELEM(quad* q);
void generate_TABLESETELEM(quad* q);
void generate_NOP(quad* q);
void generate_JUMP(quad* q);
void generate_PARAM(quad* q);
void generate_GETRETVAL(quad* q);
void generate_RETURN(quad* q);
typedef void (*generator_func_t)(quad*);