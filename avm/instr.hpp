#ifndef INSTR
#define INSTR

#include <string>

#include "avm_stack.hpp"
#include "avm_types.hpp"

void memclear_string (avm_memcell *m);
void memclear_table (avm_memcell *m);
typedef void (*memclear_func_t) (avm_memcell*);
extern memclear_func_t memclearFuncs[];

void avm_memcellclear (avm_memcell *m);

void avm_tablesetelem (avm_table *table, avm_memcell *index, avm_memcell* content);
avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index);

#endif