#include <cassert>
#include <iostream>
#include <vector>
#include <string>

#include "avm_stack.hpp"
#include "avm_types.hpp"
#include "avm_dispatch.hpp"
#include "avm_loader.hpp"
#include "instr.hpp"
#include <cassert>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>

extern avm_memcell ax, bx, cx, retval;
extern avm_memcell stack[STACK_SZ];
extern unsigned esp, ebp;
unsigned char executionFinished = 0;
unsigned pc = 0;
unsigned currLine = 0;

typedef void (*execute_func_t)(instruction*);

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
    execute_nop,
    execute_ret,
    execute_getretval,
    execute_jump
};

void executeCycle (void) {
    if (executionFinished) { std::cout << "exefin\n"; return; }
    else if (pc == AVM_ENDING_PC) {
        if(debug_flag != 0){
            std::cout << "pc == avm ending pc\n";
        }
        executionFinished = 1;
        return;
    }
    else {
        assert(pc < AVM_ENDING_PC);
        // instruction *instr = code + pc;
        instruction *instr = instructions + pc;
        assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
        if(debug_flag != 0){
            std::cout << "VM fetching pc:" << pc << "opcode: " << instr->opcode << ".\n";
        }
        if (instr->srcline)
            currLine = instr->srcline;
        unsigned oldPc = pc;
        (*executeFuncs[instr->opcode]) (instr);
        if(pc == oldPc) {++pc;}
    }
}

/*-------------arithmetic-------------*/
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

    // assert(lv && (&stack[STACK_SZ - 1] >= lv && lv > &stack[esp]));
    assert(lv && (&stack[STACK_SZ - 1] >= lv && lv >= &stack[esp]));
    assert(rv1 && rv2);

    if (rv1->type != number_m || rv2->type != number_m) {
        printf("not a number in arithmetic\n");
        executionFinished = 1;
    }else {
        if((i->opcode == div_v)||(i->opcode == mod_v)){
            if(rv2->data.numVal == 0.0){
                printf("Error: Division or modulo by zero!!\n");
                executionFinished = 1;
                return;
            }
        }
        arithmetic_func_t op = arithmeticFuncs[i->opcode - add_v];
        avm_memcellclear(lv);
        lv->type = number_m;
        lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
    }
}

void execute_uminus(instruction* i){
    avm_memcell* lv = avm_translate_operand(&i->result, (avm_memcell*)0);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);
    
    assert(lv && rv1);

    if(rv1->type != number_m){
        printf("Error: Uminus applied to non-number.\n");
        executionFinished=1;
    }else{
        avm_memcellclear(lv);
        lv->type = number_m;
        lv->data.numVal = -rv1->data.numVal;
    }
}

void execute_assign(instruction *instr) 
{
    avm_memcell *lv = avm_translate_operand(&instr->result, (avm_memcell*) 0);
    avm_memcell *rv = avm_translate_operand(&instr->arg1, &ax);
    avm_assign(lv, rv);
}

void execute_newtable(instruction* i)
{
    avm_memcell* lv = avm_translate_operand(&i->result, 0);
    // assert(lv && (((&stack[STACK_SZ - 1] >= lv) && (&stack[esp] < lv)) || (lv == &retval)));
    assert(lv && (((&stack[STACK_SZ - 1] >= lv) && (&stack[esp] <= lv)) || (lv == &retval)));
    avm_memcellclear(lv);

    lv->type = table_m;
    lv->data.tableVal = new avm_table();
    lv->data.tableVal->incrrefcounter();
}

void execute_tablegetelem(instruction* instr)
{
    avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell*)0);
    avm_memcell* t = avm_translate_operand(&instr->arg1, (avm_memcell*)0);
    avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);

    assert(lv && &stack[STACK_SZ - 1] >= lv && lv >= &stack[esp]);
    assert(t && &stack[STACK_SZ - 1] >= t && t >= &stack[esp]);

    if(t->type != table_m){
        printf("Error: Illegal use of type %s as table\n",typeStrings[t->type]);
        executionFinished=1;
        return;
    }
    avm_memcell* content = avm_tablegetelem(t->data.tableVal, i);

    if (content) {
        avm_assign(lv, content);
    }
    else {
        std::cout<<"WARNING: table["<<avm_tostring(i)<<"] not found! at line "<<currLine<<"\n";
        avm_memcellclear(lv);
        lv->type = nil_m;
    }
}

void execute_tablesetelem(instruction* instr)
{
    avm_memcell* t = avm_translate_operand(&instr->result, (avm_memcell*)0);
    avm_memcell* i = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell* c = avm_translate_operand(&instr->arg2, &bx);

    assert(t);
    assert(i && c);

    if (t->type != table_m){
        std::cout << "illegal use of type " << typeStrings[t->type] << " as table\n";
    }
    else {
        avm_tablesetelem(t->data.tableVal, i, c);
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

unsigned char avm_tobool(avm_memcell* m){
    assert((m->type >= 0)&&(m->type <= undef_m));
    return toboolFuncs[m->type](m);
}

/*-------------logical-------------*/
void execute_and(instruction* i){
    avm_memcell* lv = avm_translate_operand(&i->result, (avm_memcell*)0);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&i->arg2, &bx);

    avm_memcellclear(lv);
    lv->type = bool_m;
    lv->data.boolVal = avm_tobool(rv1) && avm_tobool(rv2);
}

void execute_or(instruction* i){
    avm_memcell* lv = avm_translate_operand(&i->result, (avm_memcell*)0);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&i->arg2, &bx);

    avm_memcellclear(lv);
    lv->type = bool_m;
    lv->data.boolVal = avm_tobool(rv1) || avm_tobool(rv2);
}

void execute_not(instruction* i){
    avm_memcell* lv = avm_translate_operand(&i->result, (avm_memcell*)0);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);

    avm_memcellclear(lv);
    lv->type = bool_m;
    lv->data.boolVal = !avm_tobool(rv1);
}

/*-------------relational-------------*/
void execute_jeq (instruction* instr) {
    assert(instr->result.type == label_a);

    avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;

    if(rv1->type == undef_m || rv2->type == undef_m){
        printf("Error: 'undef' involved in equality\n");
        executionFinished=1;
    }else if (rv1->type == bool_m || rv2->type == bool_m){
        result = (avm_tobool(rv1) == avm_tobool(rv2));
    }else if (rv1->type == nil_m || rv2->type == nil_m){
        result = rv1->type == nil_m && rv2->type == nil_m;
    }else if (rv1->type != rv2->type){
        printf("Error: %s == %s is illegal\n", typeStrings[rv1->type], typeStrings[rv2->type]);
        executionFinished=1;
    }else {
        switch(rv1->type){
            case number_m:
                result = (std::abs(rv1->data.numVal - rv2->data.numVal) < 1e-5);
                break;
            case string_m:
                result = (strcmp(rv1->data.strVal, rv2->data.strVal)==0);
                break;
            case table_m:
                result = (rv1->data.tableVal == rv2->data.tableVal);
                break;
            case userfunc_m:
                result = (rv1->data.funcVal == rv2->data.funcVal);
                break;
            case libfunc_m:
                result = (strcmp(rv1->data.libFuncVal, rv2->data.libFuncVal)==0);
                break;
            default:
                result = 0;
                break;
        }
    }

    if (!executionFinished && result){
        pc = instr->result.val;
    }
}

void execute_jne(instruction* instr){
    assert(instr->result.type == label_a);

    avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;

    if(rv1->type == undef_m || rv2->type == undef_m){
        printf("Error: 'undef' involved in equality\n");
        executionFinished=1;
    }else if (rv1->type == bool_m || rv2->type == bool_m){
        result = (avm_tobool(rv1) != avm_tobool(rv2));
    }else if (rv1->type == nil_m || rv2->type == nil_m){
        result = !(rv1->type == nil_m && rv2->type == nil_m);
    }else if (rv1->type != rv2->type){
        printf("Error: %s != %s is illegal\n", typeStrings[rv1->type], typeStrings[rv2->type]);
        executionFinished=1;
    }else {
        switch(rv1->type){
            case number_m:
                result = (std::abs(rv1->data.numVal - rv2->data.numVal) >= 1e-5);
                break;
            case string_m:
                result = (strcmp(rv1->data.strVal, rv2->data.strVal) != 0);
                break;
            case table_m:
                result = (rv1->data.tableVal != rv2->data.tableVal);
                break;
            case userfunc_m:
                result = (rv1->data.funcVal != rv2->data.funcVal);
                break;
            case libfunc_m:
                result = (strcmp(rv1->data.libFuncVal, rv2->data.libFuncVal) != 0);
                break;
            default:
                result = 1;
                break;
        }
    }

    if (!executionFinished && result){
        pc = instr->result.val;
    }
}

void execute_jle(instruction* i){
    assert(i->result.type == label_a);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&i->arg2, &bx);

    // if((rv1->type != number_m)||(rv2->type != number_m)){
    //     printf("Error: Relational operator on non-numbers.\n");
    //     executionFinished=1;
    // }else if(rv1->data.numVal <= rv2->data.numVal){
    //     pc = i->result.val;
    // }
    if((rv1->type == number_m)&&(rv2->type == number_m)){
        if(rv1->data.numVal <= rv2->data.numVal){
            pc = i->result.val;
        }
    }
}

void execute_jge(instruction* i){
    assert(i->result.type == label_a);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&i->arg2, &bx);

    // if((rv1->type != number_m)||(rv2->type != number_m)){
    //     printf("Error: Relational operator on non-numbers.\n");
    //     executionFinished=1;
    // }else if(rv1->data.numVal >= rv2->data.numVal){
    //     pc = i->result.val;
    // }
    if((rv1->type == number_m)&&(rv2->type == number_m)){
        if(rv1->data.numVal >= rv2->data.numVal){
            pc = i->result.val;
        }
    }
}

void execute_jlt(instruction* i){
    assert(i->result.type == label_a);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&i->arg2, &bx);

    // if((rv1->type != number_m)||(rv2->type != number_m)){
    //     printf("Error: Relational operator on non-numbers.\n");
    //     executionFinished=1;
    // }else if(rv1->data.numVal < rv2->data.numVal){
    //     pc = i->result.val;
    // }
    if((rv1->type == number_m)&&(rv2->type == number_m)){
        if(rv1->data.numVal < rv2->data.numVal){
            pc = i->result.val;
        }
    }
}

void execute_jgt(instruction* i){
    assert(i->result.type == label_a);
    avm_memcell* rv1 = avm_translate_operand(&i->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&i->arg2, &bx);

    // if((rv1->type != number_m)||(rv2->type != number_m)){
    //     printf("Error: Relational operator on non-numbers.\n");
    //     executionFinished=1;
    // }else if(rv1->data.numVal > rv2->data.numVal){
    //     pc = i->result.val;
    // }
    if((rv1->type == number_m)&&(rv2->type == number_m)){
        if(rv1->data.numVal > rv2->data.numVal){
            pc = i->result.val;
        }
    }
}

/*-------------jumps and returns-------------*/
void execute_jump(instruction* instr){
    assert(instr->result.type == label_a);
    if(executionFinished == 0){
        pc = instr->result.val;
    }
}

void execute_ret(instruction* instr){}

void execute_getretval(instruction* instr){
    avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell*)0);
    assert(lv);
    avm_assign(lv, &retval);
}

/*-------------tables-------------*/
// void execute_newtable(instruction* i){
//     avm_memcell* lv = avm_translate_operand(&i->result, (avm_memcell*)0);
//     assert(lv && (((&stack[STACK_SZ - 1] >= lv) && (&stack[esp] < lv)) || (lv == &retval)));
//     avm_memcellclear(lv);

//     lv->type = table_m;
//     lv->data.tableVal = new avm_table();
//     lv->data.tableVal->incrrefcounter();
// }

// void execute_tablegetelem(instruction* instr){
//     avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell*)0);
//     avm_memcell* t = avm_translate_operand(&instr->arg1, (avm_memcell*)0);
//     avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);

//     assert(lv && &stack[STACK_SZ - 1] >= lv && lv > &stack[esp]);
//     assert(t && &stack[STACK_SZ - 1] >= t && t > &stack[esp]);

//     avm_memcellclear(lv);
//     lv->type = nil_m;

//     if(t->type != table_m){
//         printf("Error: Illegal use of type %s as table\n", typeStrings[t->type]);
//         executionFinished = 1;
//     }else{
//         //ANDREA DO THIS <3
//     }
// }

/*-------------remainings-------------*/
void execute_call(instruction *instr){
    avm_memcell* func = avm_translate_operand(&instr->arg1, &ax);
    assert(func);

    switch (func->type) {
        case userfunc_m : {
            avm_callsaveenviroment();
            // pc = func->data.funcVal;

            if(executionFinished) return;

            userfunc* finfo = avm_getfuncinfo_byindex(func->data.funcVal);
            assert(finfo);
            pc = finfo->address;

            assert(pc < AVM_ENDING_PC);
            assert(instructions[pc].opcode == funcenter_v);
            break;
        }
        case string_m:
            avm_calllibfunc(func->data.strVal);
            break;
        case libfunc_m:
            avm_calllibfunc(func->data.libFuncVal);
            break;
        case table_m:
            avm_call_functor(func->data.tableVal);
            break;

        default: {
            std::string s = avm_tostring(func);
            std::cout << "call: can not bind ' "<< s << "' to function\n";
            executionFinished = 1;
        }
    }
}

extern unsigned totalActuals;
void execute_pusharg (instruction* instr)
{
    avm_memcell* arg = avm_translate_operand(&instr->arg1, &ax);
    assert(arg);

    avm_assign(&stack[esp], arg);
    ++totalActuals;
    avm_dec_top();
}


void execute_funcenter(instruction *instr)
{
    avm_memcell* func = avm_translate_operand(&instr->result, &ax);
    assert(func);
    // assert (pc == func->data.funcVal);

    userfunc* funcInfo = avm_getfuncinfo_byindex(func->data.funcVal);
    assert(funcInfo);
    assert(pc == funcInfo->address);

    totalActuals = 0;
    //userfunc *funcInfo = avm_getfuncinfo(pc);
    ebp = esp;
    esp = esp - funcInfo->localsize;
}


void execute_funcexit(instruction* i)
{
    unsigned oldTop = esp;
    esp = avm_get_envvalue(ebp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(ebp + AVM_SAVEDPC_OFFSET);
    ebp = avm_get_envvalue(ebp + AVM_SAVEDTOPSP_OFFSET);

    // while (oldTop++ <= esp){
    //     avm_memcellclear(&stack[oldTop]);
    // }
    for(unsigned i=oldTop+1 ; i<= esp; ++i){
        avm_memcellclear(&stack[i]);
    }
}


void execute_nop(instruction* i){}