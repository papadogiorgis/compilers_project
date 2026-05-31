#include <iostream>

#include "instr.hpp"
#include "avm_dispatch.hpp"
#include "avm_stack.hpp"
#include "avm_types.hpp"
#include "avm_loader.hpp"

extern bool debug_flag;
extern unsigned char executionFinished;
extern unsigned pc;

int main(int argc, char* argv[]){
    if(!load_binary(argc, argv)){
        return -1;
    }

    avm_initfuncs();
    avm_initialize_stack();
    while(!executionFinished) {
        executeCycle();
    }

    return 0;
}