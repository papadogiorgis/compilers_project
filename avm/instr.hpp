#ifndef INSTR
#define INSTR
#include "avm_stack.hpp"
#include "avm_types.hpp"

extern void memclear_string (avm_memcell *m);
extern void memclear_table (avm_memcell *m);
typedef void (*memclear_func_t) (avm_memcell*);
memclear_func_t memclearFuncs[] = {
    0, // number
    memclear_string,
    0, // bool
    memclear_table,
    0,  // userfunc
    0,  // libfunc
    0,  // nil
    0   // undef
};

void avm_memcellclear (avm_memcell *m);

#endif