#include "avm_loader.hpp"

extern bool debug_flag;

int main(int argc, char* argv[]){
    if(!load_binary(argc, argv)){
        return -1;
    }

    return 0;
}