#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include "../instr.hpp"
#include "../avm_stack.hpp"
#include "../avm_types.hpp"
#include "functions.hpp"

void avm_registerlibfunc (char* id, library_func_t addr);
extern avm_memcell ax, bx, cx, retval;

void libfunc_print(void)
{
    unsigned n = avm_totalactuals();
    for (unsigned i = 0; i < n; ++i) {
        std::string s = avm_tostring(avm_getactual(i));
        std::cout << s;
    }
}

char* typeStrings[] = {
    "number",
    "string",
    "bool",
    "table",
    "userfunc",
    "libfunc",
    "nil",
    "undef"
};

void libfunc_typeof(void)
{
    unsigned n = avm_totalactuals();
    if (n != 1){
        printf("one argument expected in typeof\n");
    }
    else {
        avm_memcellclear(&retval);
        retval.type = string_m;
        retval.data.strVal = strdup(typeStrings[avm_getactual(0)->type]);
    }
}