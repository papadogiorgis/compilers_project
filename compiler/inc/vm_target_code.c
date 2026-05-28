#include "vm_target_code.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern unsigned int currQuad;
extern quad* quads;

/*initialize the extern variables declared in the .h*/
instruction* instructions = NULL;
unsigned int currInstructions=0;
unsigned int totalInstructions=0;

/*helper function to emit target instructions, similar to quad's emit()*/
void emit_instr(instruction t){
    if(currInstructions==totalInstructions){
        unsigned int size = (totalInstructions+1024)*sizeof(instruction);
        instruction* p = malloc(size);
        if(instructions){
            memcpy(p, instructions, totalInstructions*sizeof(instruction));
            free(instructions);
        }
        instructions = p;
        totalInstructions+=1024;
    }
    instructions[currInstructions++]=t;
}

/*-------make_operand() function-------------*/
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


/*-------generator array-----------------*/
generator_func_t all_generators[] = {
    generate_ASSIGN,
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_UMINUS,
    generate_AND,
    generate_OR,
    generate_NOT,
    generate_JUMP,
    generate_JEQ,
    generate_JNE,
    generate_JLE,
    generate_JGE,
    generate_JLT,
    generate_JGT,
    generate_CALL,
    generate_PARAM,
    generate_RETURN,
    generate_GETRETVAL,
    generate_FUNCENTER,
    generate_FUNCEXIT,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM
};

/*-----------Generators---------------------
loop through all generated quads*/
void generate_loop(void){
    for(unsigned i=1; i<currQuad; ++i){
        /*record the first target instruction generated for this quad*/
        quads[i].target_address = currInstructions;
        /*dispatch to the correct generator function based on the quad's opcode*/
        (*all_generators[quads[i].op])(&quads[i]);
    }
}

/*Helper for arithmetic, assignments, tables, and more*/
void helper_op(quad* q, enum vmopcode op){
    instruction inst;
    inst.opcode = op;
    inst.arg1.type = -1;
    inst.arg2.type = -1;
    inst.result.type = -1;
    inst.arg1.val = 0;
    inst.arg2.val = 0;
    inst.result.val = 0;
    if(q->arg1 != NULL){
        make_operand(q->arg1, &inst.arg1);
    }
    if(q->arg2 != NULL){
        make_operand(q->arg2, &inst.arg2);
    }
    if(q->result != NULL){
        make_operand(q->result, &inst.result);
    }
    inst.srcline = q->line;
    emit_instr(inst);
}

void generate_ASSIGN(quad* q){
    helper_op(q, assign_v);
}
void generate_ADD(quad* q){
    helper_op(q, add_v);
}
void generate_SUB(quad* q){
    helper_op(q, sub_v);
}
void generate_MUL(quad* q){
    helper_op(q, sub_v);
}
void generate_DIV(quad* q){
    helper_op(q, div_v);
}
void generate_MOD(quad* q){
    helper_op(q, mod_v);
}
void generate_UMINUS(quad* q){
    helper_op(q, uminus_v);
}
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