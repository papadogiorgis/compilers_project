#ifndef VM_TARGET_CODE
#define VM_TARGET_CODE

#include "quads.h"

extern instruction* instructions;
extern unsigned int currInstructions;
extern unsigned int totalInstructions;

enum vmopcode {
    assign_v, add_v, sub_v, mul_v, div_v,
    mod_v, uminus_v, and_v, or_v, not_v, jeq_v,
    jne_v, jle_v, jge_v, jlt_v, jgt_v, call_v, 
    pusharg_v, funcenter_v, funcexit_v, newtable_v,
    tablegetelem_v, tablesetelem_v, nop_v,
    ret_v, getretval_v, jump_v
};

enum vmarg_t{
    label_a=0,
    global_a=1,
    formal_a=2,
    local_a=3,
    number_a=4,
    string_a=5,
    bool_a=6,
    nil_a=7,
    userfunc_a=8,
    libfunc_a=9,
    retval_a=10,
};

typedef struct vmarg {
    enum vmarg_t type;
    unsigned val;
} vmarg ;

typedef struct instruction {
    enum vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg  arg2;
    unsigned srcline;
} instruction;

typedef struct userfunc {
    unsigned address;
    unsigned localsize;
    char *id;
} userfunc;

double* numConsts;
unsigned totalNumConsts;
char** stringConsts;
unsigned totalStringConsts;
char** namedLibfuncs;
unsigned totalNamedLibfuncs;
userfunc* userFuncs;
unsigned totalUserFuncs;

typedef struct incomplete_jump{
    unsigned instrNo;
    unsigned iaddress;
    incomplete_jump* next;
}incomplete_jump;
incomplete_jump* ij_head = (incomplete_jump*)0;
unsigned ij_total = 0;
void add_incomplete_jump(unsigned instrNo, unsigned iaddress);
void patch_incomplete_jumps(void);

void generate_loop(void);

void make_operand(expr* e, vmarg* arg);

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

#endif