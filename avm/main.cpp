#include <iostream>

#include "instr.hpp"
#include "avm_dispatch.hpp"
#include "avm_stack.hpp"
#include "avm_types.hpp"
#include "avm_loader.hpp"

extern bool debug_flag;
extern unsigned char executionFinished;
extern unsigned pc;
extern unsigned esp;

int main(int argc, char* argv[]){
    avm_initialize_stack();
    avm_initfuncs();
    if(!load_binary(argc, argv)){
        return -1;
    }

    /*shift esp down to reserve space for globals/temps*/
    esp = esp-programVarOffset;

    if(debug_flag != 0){
        std::cout << "total instr: " << totalInstructions <<"\n";
    }

    while(!executionFinished) {
        executeCycle();
    }

    return 0;
}