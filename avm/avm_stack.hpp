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

#endif