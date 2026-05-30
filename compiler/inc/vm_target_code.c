#include "vm_target_code.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern unsigned int currQuad;
extern quad* quads;
extern int print_syntax;

/*initialize the extern variables declared in the .h*/
instruction* instructions = NULL;
unsigned int currInstructions=0;
unsigned int totalInstructions=0;
double* numConsts=NULL;
unsigned totalNumConsts=0;
char** stringConsts=NULL;
unsigned totalStringConsts=0;
char** namedLibfuncs=NULL;
unsigned totalNamedLibfuncs=0;
userfunc* userFuncs=NULL;
unsigned totalUserFuncs=0;

incomplete_jump* ij_head = NULL;
unsigned ij_total = 0;

extern unsigned int programVarOffset; //thats from quads.c

funcjump_stack* funcjump_top;

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

/*-------constant tables and incomplete jumps-----------------*/
unsigned consts_newnumber(double n){
    for(unsigned i=0; i<totalNumConsts; i++){
        if(numConsts[i]==n){
            return i;
        }
    }
    numConsts = realloc(numConsts, (totalNumConsts+1)*sizeof(double));
    numConsts[totalNumConsts]=n;
    return totalNumConsts++;
}

unsigned consts_newstring(char* s){
    for(unsigned i=0; i<totalStringConsts; i++){
        if(strcmp(stringConsts[i],s)==0){
            return i;
        }
    }
    stringConsts = realloc(stringConsts, (totalStringConsts+1)*sizeof(char*));
    stringConsts[totalStringConsts]=strdup(s);
    return totalStringConsts++;
}

unsigned libfuncs_nevused(char* s){
    for(unsigned i=0; i<totalNamedLibfuncs; i++){
        if(strcmp(namedLibfuncs[i],s)==0){
            return i;
        }
    }
    namedLibfuncs = realloc(namedLibfuncs, (totalNamedLibfuncs+1)*sizeof(char*));
    namedLibfuncs[totalNamedLibfuncs]=strdup(s);
    return totalNamedLibfuncs++;
}

unsigned userfuncs_newfunc(node* sym){
    for(unsigned i=0; i<totalUserFuncs; i++){
        if(strcmp(userFuncs[i].id,sym->key)==0){
            return i;
        }
    }
    userFuncs = realloc(userFuncs, (totalUserFuncs+1)*sizeof(userfunc));
    userFuncs[totalUserFuncs].id=strdup(sym->key);
    userFuncs[totalUserFuncs].address=sym->iaddress;
    userFuncs[totalUserFuncs].localsize=sym->totalLocals;
    return totalUserFuncs++;
}

void add_incomplete_jump(unsigned instrNo, unsigned iaddress){
    incomplete_jump* incju = malloc(sizeof(incomplete_jump));
    incju->instrNo = instrNo;
    incju->iaddress = iaddress;
    incju->next = ij_head;
    ij_head = incju;
    ij_total++;
}

void patch_incomplete_jumps(void){
    incomplete_jump* current = ij_head;
    while(current != NULL){
        if(current->iaddress==currQuad){
            instructions[current->instrNo].result.val = currInstructions;
        }else{
            instructions[current->instrNo].result.val = quads[current->iaddress].target_address;
        }
        current = current->next;
    }
}


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
    generate_TABLESETELEM,
    generate_NOP
};

/*-----------Generators---------------------*/

/*Helper for arithmetic, assignments, tables, logical*/
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
    helper_op(q, mul_v);
}
void generate_DIV(quad* q){
    helper_op(q, div_v);
}
void generate_MOD(quad* q){
    helper_op(q, mod_v);
}
void generate_UMINUS(quad* q){
    /*MULTIPLICATION WITH -1*/
    instruction inst;
    inst.opcode = mul_v;
    inst.arg1.type = -1;
    inst.arg2.type = -1;
    inst.result.type = -1;
    inst.arg1.val = 0;
    inst.arg2.val = 0;
    inst.result.val = 0;
    
    make_operand(q->arg1, &inst.arg1);
    make_operand(q->result, &inst.result);
    inst.arg2.type = number_a;
    inst.arg2.val = consts_newnumber(-1.0);
    inst.srcline = q->line;
    emit_instr(inst);
}
void generate_AND(quad* q){
    helper_op(q, and_v);
}
void generate_OR(quad* q){
    helper_op(q, or_v);
}
void generate_NOT(quad* q){
    helper_op(q, not_v);
}
void generate_NEWTABLE(quad* q){
    helper_op(q, newtable_v);
}
void generate_TABLEGETELEM(quad* q){
    helper_op(q, tablegetelem_v);
}
void generate_TABLESETELEM(quad* q){
    helper_op(q, tablesetelem_v);
}
void generate_NOP(quad* q){
    helper_op(q, nop_v);
}

/*Helper for jumps*/
void helper_jumps(quad* q, enum vmopcode op){
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
    inst.result.type = label_a;
    inst.srcline = q->line;
    emit_instr(inst);
    add_incomplete_jump(currInstructions-1, q->label);
}

void generate_JEQ(quad* q){
    helper_jumps(q, jeq_v);
}
void generate_JNE(quad* q){
    helper_jumps(q, jne_v);
}
void generate_JLE(quad* q){
    helper_jumps(q, jle_v);
}
void generate_JGE(quad* q){
    helper_jumps(q, jge_v);
}
void generate_JLT(quad* q){
    helper_jumps(q, jlt_v);
}
void generate_JGT(quad* q){
    helper_jumps(q, jgt_v);
}
void generate_JUMP(quad* q){
    helper_jumps(q, jump_v);
}

void generate_CALL(quad* q){
    q->target_address = currInstructions;
    instruction inst;
    inst.opcode = call_v;
    inst.arg1.type = -1;
    inst.arg2.type = -1;
    inst.result.type = -1;
    inst.arg1.val = 0;
    inst.arg2.val = 0;
    inst.result.val = 0;

    make_operand(q->arg1, &inst.arg1);
    inst.srcline = q->line;
    emit_instr(inst);
}

/*simple stack to track function jump patching for nested functions*/

void push_funcjump(unsigned instrNo){
    funcjump_stack* temp;
    if(funcjump_top==NULL){
        funcjump_top = malloc(sizeof(struct funcjump_stack));
        funcjump_top->prev = NULL;
        assert(funcjump_top);
    }else{
        funcjump_top->next = malloc(sizeof(struct funcjump_stack));
        assert(funcjump_top->next);
        temp = funcjump_top;
        funcjump_top = funcjump_top->next;
        funcjump_top->prev = temp;
    }
    funcjump_top->next = NULL;
    funcjump_top->inst = instrNo;
}

unsigned pop_funcjump(){
    unsigned temp_num;
    funcjump_stack* temp;
    if(funcjump_top==NULL){
        printf("Error: Cannot pop from empty funcjump_stack.\n");
        return 0;
    }
    temp_num = funcjump_top->inst;
    temp = funcjump_top;
    if(funcjump_top->prev==NULL){
        funcjump_top = NULL;
    }else{
        funcjump_top = funcjump_top->prev;
    }
    free(temp);
    return temp_num;
}

void generate_FUNCENTER(quad* q){
    /*emit jump to skip function body*/
    instruction jump_inst;
    jump_inst.opcode = jump_v;
    jump_inst.arg1.type = -1;
    jump_inst.arg2.type = -1;
    jump_inst.result.type = label_a;
    jump_inst.arg1.val = 0;
    jump_inst.arg2.val = 0;
    jump_inst.result.val = 0;
    jump_inst.srcline = q->line;
    emit_instr(jump_inst);

    push_funcjump(currInstructions-1);

    instruction inst;
    inst.opcode = funcenter_v;
    inst.arg1.type = -1;
    inst.arg2.type = -1;
    inst.result.type = -1;
    inst.arg1.val = 0;
    inst.arg2.val = 0;
    inst.result.val = 0;

    make_operand(q->result, &inst.result);
    inst.srcline = q->line;
    emit_instr(inst);
}

void generate_FUNCEXIT(quad* q){
    instruction inst;
    inst.opcode = funcexit_v;
    inst.arg1.type = -1;
    inst.arg2.type = -1;
    inst.result.type = -1;
    inst.arg1.val = 0;
    inst.arg2.val = 0;
    inst.result.val = 0;

    make_operand(q->result, &inst.result);
    inst.srcline = q->line;
    emit_instr(inst);

    unsigned jump_index = pop_funcjump();
    instructions[jump_index].result.val = currInstructions;
}

void generate_PARAM(quad* q){
    q->target_address = currInstructions;
    instruction inst;
    inst.opcode = pusharg_v;
    inst.arg1.type = -1;
    inst.arg2.type = -1;
    inst.result.type = -1;
    inst.arg1.val = 0;
    inst.arg2.val = 0;
    inst.result.val = 0;

    make_operand(q->arg1, &inst.arg1);
    inst.srcline = q->line;
    emit_instr(inst);
}

void generate_GETRETVAL(quad* q){
    q->target_address = currInstructions;
    instruction inst;
    inst.opcode = assign_v;
    inst.arg1.type = -1;
    inst.arg2.type = -1;
    inst.result.type = -1;
    inst.arg1.val = 0;
    inst.arg2.val = 0;
    inst.result.val = 0;

    make_operand(q->result, &inst.result);
    inst.arg1.type = retval_a;
    inst.srcline = q->line;
    emit_instr(inst);
}

void generate_RETURN(quad* q){
    instruction inst;
    inst.opcode = assign_v;
    inst.arg1.type = -1;
    inst.arg2.type = -1;
    inst.result.type = -1;
    inst.arg1.val = 0;
    inst.arg2.val = 0;
    inst.result.val = 0;

    inst.result.type = retval_a;
    if(q->arg1 != NULL){
        make_operand(q->arg1, &inst.arg1);
    }
    inst.srcline = q->line;
    emit_instr(inst);
}

// void generate_PUSHARG(quad* q);

/*----------for debugging-------------*/
void print_op(enum vmopcode op){
    if(op==assign_v){
        printf("assign ");
    }else if(op==add_v){
        printf("add ");
    }else if(op==sub_v){
        printf("sub ");
    }else if(op==mul_v){
        printf("mul ");
    }else if(op==div_v){
        printf("div ");
    }else if(op==mod_v){
        printf("mod ");
    }else if(op==uminus_v){
        printf("uminus ");
    }else if(op==and_v){
        printf("and ");
    }else if(op==or_v){
        printf("or ");
    }else if(op==not_v){
        printf("not ");
    }else if(op==jeq_v){
        printf("jeq ");
    }else if(op==jne_v){
        printf("jne ");
    }else if(op==jle_v){
        printf("jle ");
    }else if(op==jge_v){
        printf("jge ");
    }else if(op==jlt_v){
        printf("jlt ");
    }else if(op==jgt_v){
        printf("jgt ");
    }else if(op==call_v){
        printf("call ");
    }else if(op==pusharg_v){
        printf("pusharg ");
    }else if(op==funcenter_v){
        printf("funcenter ");
    }else if(op==funcexit_v){
        printf("funcexit ");
    }else if(op==newtable_v){
        printf("newtable ");
    }else if(op==tablegetelem_v){
        printf("tablegetelem ");
    }else if(op==tablesetelem_v){
        printf("tablesetelem ");
    }else if(op==nop_v){
        printf("nop ");
    }else if(op==ret_v){
        printf("return ");
    }else if(op==getretval_v){
        printf("getretval ");
    }else if(op==jump_v){
        printf("jump ");
    }else{
        printf("UNKNOWN_OP ");
    }
}

void printarg(vmarg* varg){
    if((varg==NULL)||((int)varg->type==-1)){
        return;
    }
    if(varg->type==label_a){
        printf("label:");
    }else if(varg->type==global_a){
        printf("global:");
    }else if(varg->type==formal_a){
        printf("formal:");
    }else if(varg->type==local_a){
        printf("local:");
    }else if(varg->type==number_a){
        printf("number:");
    }else if(varg->type==string_a){
        printf("string:");
    }else if(varg->type==bool_a){
        printf("bool:");
    }else if(varg->type==nil_a){
        printf("nil:");
    }else if(varg->type==userfunc_a){
        printf("userfunc:");
    }else if(varg->type==libfunc_a){
        printf("libfunc:");
    }else if(varg->type==retval_a){
        printf("retval:");
    }else{
        printf("UNKNOWN_TYPE ");
    }
    printf("%d ",varg->val);
}

/*loop through all generated quads*/
void generate_loop(void){
    if(print_syntax){
        printf("\n\n----------------TARGET CODE----------------\n");
    }
    for(unsigned i=1; i<currQuad; ++i){
        /*record the first target instruction generated for this quad*/
        quads[i].target_address = currInstructions;
        /*dispatch to the correct generator function based on the quad's opcode*/
        (*all_generators[quads[i].op])(&quads[i]);
    }

    patch_incomplete_jumps();

    if(print_syntax){
        for(unsigned i=0; i<currInstructions; ++i){
            printf("%d | ",i);
            print_op(instructions[i].opcode);
            printarg(&instructions[i].arg1);
            printarg(&instructions[i].arg2);
            printarg(&instructions[i].result);
            printf("\n");
        }
        if(numConsts!=NULL){
            printf("----------------numConsts----------------\n");
            for(int k=0; k<totalNumConsts; k++){
                printf("%d | %lf\n",k,numConsts[k]);
            }
        }
        if(stringConsts!=NULL){
            printf("----------------stringConsts----------------\n");
            for(int k=0; k<totalStringConsts; k++){
                printf("%d | \"%s\"\n",k,stringConsts[k]);
            }
        }
        if(namedLibfuncs!=NULL){
            printf("----------------namedLibfuncs----------------\n");
            for(int k=0; k<totalNamedLibfuncs; k++){
                printf("%d | \"%s\"\n",k,namedLibfuncs[k]);
            }
        }
        if(userFuncs!=NULL){
            printf("----------------userFuncs----------------\n");
            for(int k=0; k<totalUserFuncs; k++){
                printf("%d | \"%s\" | address=%d, localsize=%d\n",k,userFuncs[k].id,userFuncs[k].address,userFuncs[k].localsize);
            }
        }
    }
}

/*----------------generate binary file---------------------*/
void create_binary_file(void){
    FILE* out_abc = fopen("out.abc","wb");
    if(!out_abc){
        printf("Couldn't open file abc.out\n");
        return;
    }

    unsigned int magic_num = 340200499;
    fwrite(&magic_num, sizeof(unsigned int), 1, out_abc);
    fwrite(&programVarOffset, sizeof(unsigned int),1,out_abc);
    fwrite(&totalStringConsts, sizeof(unsigned),1,out_abc);
    for(unsigned i=0; i<totalStringConsts; ++i){
        unsigned str_length = strlen(stringConsts[i])+1;
        fwrite(&str_length, sizeof(unsigned),1,out_abc);
        fwrite(stringConsts[i],sizeof(char),str_length,out_abc);
    }
    fwrite(&totalNumConsts, sizeof(unsigned),1,out_abc);
    for(unsigned i=0; i<totalNumConsts; ++i){
        fwrite(&numConsts[i],sizeof(double),1,out_abc);
    }
    fwrite(&totalUserFuncs, sizeof(unsigned),1,out_abc);
    for(unsigned i=0; i<totalUserFuncs; ++i){
        fwrite(&userFuncs[i].address,sizeof(unsigned),1,out_abc);
        fwrite(&userFuncs[i].localsize,sizeof(unsigned),1,out_abc);
        unsigned usfunclength = strlen(userFuncs[i].id)+1;
        fwrite(&usfunclength,sizeof(unsigned),1,out_abc);
        fwrite(userFuncs[i].id,sizeof(char),usfunclength,out_abc);
    }
    fwrite(&totalNamedLibfuncs,sizeof(unsigned),1,out_abc);
    for(unsigned i=0; i<totalNamedLibfuncs; ++i){
        unsigned libfunclength = strlen(namedLibfuncs[i])+1;
        fwrite(&libfunclength,sizeof(unsigned),1,out_abc);
        fwrite(namedLibfuncs[i],sizeof(char),libfunclength,out_abc);
    }
    fwrite(&currInstructions,sizeof(unsigned),1,out_abc);
    fwrite(instructions, sizeof(instruction),currInstructions,out_abc);

    fclose(out_abc);
}