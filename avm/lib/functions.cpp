#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include "../instr.hpp"
#include "../avm_stack.hpp"
#include "../avm_types.hpp"
#include "functions.hpp"

void avm_registerlibfunc (char* id, library_func_t addr);
extern avm_memcell ax, bx, cx, retval;

extern const char* typeStrings[];

void libfunc_print(void)
{
    unsigned n = avm_totalactuals();
    std::cout << n << "\n";
    for (unsigned i = 0; i < n; ++i) {
        std::string s = avm_tostring(avm_getactual(i));
        std::cout << s;
    }
    std::cout.flush();
}

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

void libfunc_input(void){}
void libfunc_objectmemberkeys(void){}
void libfunc_objecttotalmembers(void){}
void libfunc_objectcopy(void){}
void libfunc_totalarguments(void){}
void libfunc_argument(void){}
void libfunc_strtonum(void){}
void libfunc_sqrt(void){}
void libfunc_cos(void){}
void libfunc_sin(void){}