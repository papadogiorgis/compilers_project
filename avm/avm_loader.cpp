#include "avm_loader.hpp"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

instruction* instructions = nullptr;
unsigned int totalInstructions=0;
unsigned int currInstructions=0;
double* numConsts=nullptr;
unsigned totalNumConsts=0;
char** stringConsts=nullptr;
unsigned totalStringConsts=0;
char** namedLibfuncs=nullptr;
unsigned totalNamedLibfuncs=0;
userfunc* userFuncs = nullptr;
unsigned totalUserFuncs=0;
unsigned programVarOffset=0;

bool debug_flag = false;

void print_op(enum vmopcode op);
void printarg(vmarg* varg);

bool load_binary(int argc, char* argv[]){
    char* filename = nullptr;
    
    if(argc<2 || argc>3){
        std::cerr<<"Usage "<<argv[0]<<" -debug-flag[optional] <binary_file>\n";
        return false;
    }

    for(int i=1; i<argc; i++){
        if(strcmp(argv[i], "-debug-flag")==0){
            debug_flag=true;
        }else{
            if(filename!=nullptr){
                std::cerr<<"Too many arguments provided!\n";
                return false;
            }
            filename=argv[i];
        }
    }

    if(filename==nullptr){
        std::cerr<<"Give binary file.\n";
        return false;
    }

    FILE* f = fopen(filename, "rb");
    if(!f){
        std::cerr<<"Could not open binary file "<<filename<<"\n";
        return false;
    }

    unsigned read_magic_num=0;
    fread(&read_magic_num,sizeof(unsigned),1,f);
    if(read_magic_num!=340200499){
        std::cerr<<"Invalid magic number!\n";
        fclose(f);
        return false;
    }

    fread(&programVarOffset,sizeof(unsigned),1,f);
    fread(&totalStringConsts,sizeof(unsigned),1,f);
    stringConsts = new char*[totalStringConsts];
    for(unsigned i=0; i<totalStringConsts; ++i){
        unsigned strlength;
        fread(&strlength, sizeof(unsigned),1,f);
        stringConsts[i]= new char[strlength];
        fread(stringConsts[i],sizeof(char),strlength,f);
    }

    fread(&totalNumConsts,sizeof(unsigned),1,f);
    numConsts= new double[totalNumConsts];
    for(unsigned i=0; i<totalNumConsts; ++i){
        fread(&numConsts[i], sizeof(double),1,f);
    }

    fread(&totalUserFuncs, sizeof(unsigned),1,f);
    userFuncs = new userfunc[totalUserFuncs];
    for(unsigned i=0; i<totalUserFuncs; ++i){
        fread(&userFuncs[i].address, sizeof(unsigned),1,f);
        fread(&userFuncs[i].localsize, sizeof(unsigned),1,f);
        unsigned usfunclength;
        fread(&usfunclength, sizeof(unsigned),1,f);
        userFuncs[i].id= new char[usfunclength];
        fread(userFuncs[i].id, sizeof(char),usfunclength,f);
    }

    fread(&totalNamedLibfuncs,sizeof(unsigned),1,f);
    namedLibfuncs = new char*[totalNamedLibfuncs];
    for(unsigned i=0; i<totalNamedLibfuncs; ++i){
        unsigned libfunclength;
        fread(&libfunclength,sizeof(unsigned),1,f);
        namedLibfuncs[i] = new char[libfunclength];
        fread(namedLibfuncs[i],sizeof(char),libfunclength,f);
    }

    fread(&totalInstructions,sizeof(unsigned),1,f);
    instructions= new instruction[totalInstructions];
    fread(instructions,sizeof(instruction),totalInstructions,f);

    fclose(f);

    if(debug_flag){
        for(unsigned i=0; i<totalInstructions; ++i){
            printf("%d | ",i);
            print_op(instructions[i].opcode);
            printarg(&instructions[i].arg1);
            printarg(&instructions[i].arg2);
            printarg(&instructions[i].result);
            printf("\n");
        }
        if(numConsts!=NULL){
            printf("----------------numConsts----------------\n");
            for(unsigned k=0; k<totalNumConsts; k++){
                printf("%d | %lf\n",k,numConsts[k]);
            }
        }
        if(stringConsts!=NULL){
            printf("----------------stringConsts----------------\n");
            for(unsigned k=0; k<totalStringConsts; k++){
                printf("%d | \"%s\"\n",k,stringConsts[k]);
            }
        }
        if(namedLibfuncs!=NULL){
            printf("----------------namedLibfuncs----------------\n");
            for(unsigned k=0; k<totalNamedLibfuncs; k++){
                printf("%d | \"%s\"\n",k,namedLibfuncs[k]);
            }
        }
        if(userFuncs!=NULL){
            printf("----------------userFuncs----------------\n");
            for(unsigned k=0; k<totalUserFuncs; k++){
                printf("%d | \"%s\" | address=%d, localsize=%d\n",k,userFuncs[k].id,userFuncs[k].address,userFuncs[k].localsize);
            }
        }
    }

    return true;
}

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