#ifndef AVM_STACK
#define AVM_STACK

#include <string>

#include "avm_types.hpp"
#include "../compiler/inc/vm_target_code.h"

#define STACK_SZ 4096
#define ENV_SZ 4

typedef std::string (*tostring_func_t)(avm_memcell*);
typedef void (*library_func_t) (void);

std::string number_tostring(avm_memcell*);
std::string string_tostring(avm_memcell*);
std::string bool_tostring(avm_memcell*);
std::string table_tostring(avm_memcell*);
std::string userfunc_tostring(avm_memcell*);
std::string libfunc_tostring(avm_memcell*);
std::string nil_tostring(avm_memcell*);
std::string undef_tostring(avm_memcell*);

unsigned avm_totalactuals (void);
avm_memcell* avm_getactual (unsigned i);


tostring_func_t tostringFuncs[]={
    number_tostring,
    string_tostring,
    bool_tostring,
    table_tostring,
    userfunc_tostring,
    libfunc_tostring,
    nil_tostring,
    undef_tostring
};

std::string avm_tostring(avm_memcell* m);
void execute_call(instruction *instr);
void avm_dec_top(void);
void avm_push_envvalue (unsigned val);
void avm_callsaveenviroment (void);
void execute_funcenter(instruction *instr);
unsigned avm_get_envvalue (unsigned i);
void execute_funcexit(instruction* instr);
void avm_call_functor (avm_table *t);
library_func_t avm_getlibfunc(std::string id);
void avm_calllibfunc(char *id);
unsigned avm_totalactuals (void);
avm_memcell* avm_getactual (unsigned i);
extern void avm_push_table_arg(avm_table* t);
void execute_pusharg (instruction* instr);
void avm_register_libfunc(std::string id, library_func_t func);
void avm_initfuncs(void);
void avm_initialize_stack(void);


#endif