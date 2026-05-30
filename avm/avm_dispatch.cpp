#include "avm_stack.hpp"
#include "avm_types.hpp"
#include "avm_dispatch.hpp"
#include "instr.hpp"
#include <cassert>

#define AVM_ENDING_PC codeSize
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v

extern avm_memcell ax, bx, cx, retval;
extern avm_memcell stack[STACK_SZ];
extern unsigned esp, ebp;
unsigned char executionFinished = 0;
unsigned pc = 0;
unsigned currLine = 0;
unsigned codeSize = 0;
instruction *code = 0;

typedef void (*execute_func_t)(instruction*);

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

double add_impl(double x, double y) { return x+y; }
double sub_impl(double x, double y) { return x-y; }
double mul_impl(double x, double y) { return x*y; }
double div_impl(double x, double y) { return x/y; }
double mod_impl(double x, double y) { 
    return ((unsigned)x) % ((unsigned)y); 
}


arithmetic_func_t arithmeticFuncs[] = {
    add_impl,
    sub_impl,
    mul_impl,
    div_impl,
    mod_impl
};

void execute_arithmetic (instruction* i)
{
    avm_memcell* lv = avm_translate_operand(&i->result, (avm_memcell*) 0);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&i->arg2, &bx);

    assert(lv && (&stack[STACK_SZ - 1] >= lv && lv > &stack[esp]));
    assert(rv1 && rv2);

    if (rv1->type != number_m || rv2->type != number_m) {
        printf("not a number in arithmetic\n");
        executionFinished = 1;
    }
    else {
        arithmetic_func_t op = arithmeticFuncs[i->opcode - add_v];
        avm_memcellclear(lv);
        lv->type = number_m;
        lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
    }
}

void execute_assign(instruction *instr) 
{
    avm_memcell *lv = avm_translate_operand(&instr->result, (avm_memcell*) 0);
    avm_memcell *rv = avm_translate_operand(&instr->arg1, &ax);

    // assert(lv && (&stack[]));

    avm_assign(lv, rv);
}

void execute_newtable(instruction* i)
{
    avm_memcell* lv = avm_translate_operand(&i->result, 0);
    assert(lv && (((&stack[STACK_SZ - 1] >= lv) && (&stack[esp] < lv)) || (lv == &retval)));
    avm_memcellclear(lv);

    lv->type = table_m;
    // lv->data.tableVal = avm_tablenew();
    //lv->data.tableVal = &avm_table();
    lv->data.tableVal = new avm_table();
    // avm_tableincrefcounter(lv->data.tableVal);
    lv->data.tableVal->incrrefcounter();
}

void execute_tablegetelem(instruction* instr)
{
    avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell*)0);
    avm_memcell* t = avm_translate_operand(&instr->arg1, (avm_memcell*)0);
    avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);

    assert(lv && &stack[STACK_SZ - 1] >= lv && lv > &stack[esp]);
    assert(t && &stack[STACK_SZ - 1] >= t && t > &stack[esp]);

    avm_memcellclear(lv);
    lv->type = nil_m;

    // if(lv->type != table_m){
    //     printf("illegal use of type %s as table", "change later");
    // }
    if(t->type!=table_m){
        printf("illegal use of type %s as table\n", typeStrings[t->type]);
        executionFinished=1;
    }else{
        (void)i;
        //TOODO
    }
}

typedef unsigned char (*tobool_func_t)(avm_memcell*);

unsigned char number_tobool (avm_memcell* m) { return m->data.numVal != 0; }
unsigned char string_tobool (avm_memcell* m) { return m->data.strVal[0] != 0; }
unsigned char bool_tobool (avm_memcell* m) { return m->data.boolVal; }
unsigned char table_tobool (avm_memcell* m) { return 1; }
unsigned char userfunc_tobool (avm_memcell* m) { return 1; }
unsigned char libfunc_tobool (avm_memcell* m) { return 1; }
unsigned char nil_tobool (avm_memcell* m) { return 0; }
unsigned char undef_tobool (avm_memcell* m) { return 0; }

tobool_func_t toboolFuncs[]={
    number_tobool,
    string_tobool,
    bool_tobool,
    table_tobool,
    userfunc_tobool,
    libfunc_tobool,
    nil_tobool,
    undef_tobool
};

void execute_jeq (instruction* instr) {
    assert(instr->result.type == label_a);

    avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;

    auto avm_tobool = [](avm_memcell *a) {
        assert(a->type >= 0 && a->type <= undef_m);
        return (toboolFuncs[a->type](a));
    };

    if (rv1->type == undef_m || rv2->type == undef_m)
        printf("'undef' involved in equality\n");
    else if (rv1->type == bool_m || rv2->type == bool_m)
        result = (avm_tobool(rv1) == avm_tobool(rv2));
    else if (rv1->type == nil_m || rv2->type == nil_m)
        result = rv1->type == nil_m && rv2->type == nil_m;
    else if (rv1->type != rv2->type){
        printf("%s == %s is illegal\n", typeStrings[rv1->type], typeStrings[rv2->type]);
    }
    else {
        // equality check with dispatching
    }

    if (!executionFinished && result){
        pc = instr->result.val;
    }
}

void execute_uminus(instruction* i){}
void execute_and(instruction* i){}
void execute_or(instruction* i){}
void execute_not(instruction* i){}
void execute_jne(instruction* i){}
void execute_jle(instruction* i){}
void execute_jge(instruction* i){}
void execute_jlt(instruction* i){}
void execute_jgt(instruction* i){}
void execute_tablesetelem(instruction* i){}
void execute_nop(instruction* i){}