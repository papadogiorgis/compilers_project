#include "vm_target_code.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void make_operand(expr* e, vmarg* arg){
    if(e == NULL){
        /*
        arg->type = -1;
        arg->val = 0;
        */
        return;
    }

    if((e->type==var_e)||(e->type==tableitem_e)||(e->type==arithexpr_e)||
    (e->type==assignexpr_e)||(e->type==boolexpr_e)||(e->type==newtable_e)){
        assert(e->sym != NULL);
        arg->val = e->sym->offset;
        if(e->sym->type == GLOBAL){
            arg->type = global_a;
        }else if(e->sym->type == LOCALV){
            arg->type = local_a;
        }else if(e->sym->type == FORMAL){
            arg->type = formal_a;
        }else{
            assert(0);
        }
    }else if(e->type == constbool_e){
        arg->val = e->boolConst;
        arg->type = bool_a;
    }else if(e->type == conststring_e){
        arg->val = consts_newstring(e->strConst);
        arg->type = string_a;
    }else if(e->type == constnum_e){
        arg->val = consts_newnumber(e->numConst);
        arg->type = number_a;
    }else if(e->type == nil_e){
        arg->type = nil_a;
    }else if(e->type == programfunc_e){
        arg->type = userfunc_a;
        arg->val = userfuncs_newfunc(e->sym);
    }else if(e->type == libraryfunc_e){
        arg->type = libfunc_a;
        arg->val = libfuncs_nevused(e->sym->key);
    }else{
        assert(0);
    }
}

// void add_incomplete_jump(unsigned instrNo, unsigned iaddress);
// void patch_incomplete_jumps(void);
// unsigned consts_newstring(char* s);
// unsigned consts_newnumber(double n);
// unsigned libfuncs_nevused(char* s);
// unsigned userfuncs_newfunc(node* sym);


/*-----------Generators---------------------*/
// void generate_loop(void){}

// void generate_ASSIGN(quad* q);
// void generate_ADD(quad* q);
// void generate_SUB(quad* q);
// void generate_MUL(quad* q);
// void generate_DIV(quad* q);
// void generate_MOD(quad* q);
// void generate_UMINUS(quad* q);
// void generate_AND(quad* q);
// void generate_OR(quad* q);
// void generate_NOT(quad* q);
// void generate_JEQ(quad* q);
// void generate_JNE(quad* q);
// void generate_JLE(quad* q);
// void generate_JGE(quad* q);
// void generate_JLT(quad* q);
// void generate_JGT(quad* q);
// void generate_CALL(quad* q);
// void generate_PUSHARG(quad* q);
// void generate_FUNCENTER(quad* q);
// void generate_FUNCEXIT(quad* q);
// void generate_NEWTABLE(quad* q);
// void generate_TABLEGETELEM(quad* q);
// void generate_TABLESETELEM(quad* q);
// void generate_NOP(quad* q);
// void generate_JUMP(quad* q);
// void generate_PARAM(quad* q);
// void generate_GETRETVAL(quad* q);
// void generate_RETURN(quad* q);
// typedef void (*generator_func_t)(quad*);