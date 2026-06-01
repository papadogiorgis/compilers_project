#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cmath>

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
    // std::cout << n << "\n";
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


extern unsigned ebp;
void libfunc_totalarguments(void)
{
    // unsigned n = avm_totalactuals();
    avm_memcellclear(&retval);
    retval.type = undef_m;

    unsigned p_topsp = avm_get_envvalue(ebp + AVM_SAVEDTOPSP_OFFSET);

    if(p_topsp == 0){
        std::cout << "totalarguments called outside of function\n";
        retval.type = nil_m;
    }
    else {
        retval.type = number_m;
        retval.data.numVal = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET); 
    }

}

void libfunc_argument(void)
{
    unsigned n = avm_totalactuals();
    avm_memcellclear(&retval);
    retval.type = undef_m;

    if (n > 1) {
        std::cout << "lib function 'argument' must be called with one argument only\n";
        retval.type = nil_m;
        return;
    }

}


void libfunc_strtonum(void)
{
    avm_memcellclear(&retval);
    unsigned n = avm_totalactuals();
    std::string s;
    for (unsigned i = 0; i < n; ++i) {
        s = avm_tostring(avm_getactual(i));
    }

    retval.type = string_m;
    retval.data.strVal = strdup(s.data());
}


void libfunc_sqrt(void)
{
    avm_memcellclear(&retval);
    unsigned n = avm_totalactuals();
    if (n > 1) {
        std::cout << "sqrt lib function must be called for one argument only\n";
        retval.type = nil_m;
        return;
    }
    retval.type = number_m;
    retval.data.numVal = std::sqrt((avm_getactual(0)->data.numVal));
}

void libfunc_cos(void)
{
    avm_memcellclear(&retval);
    unsigned n = avm_totalactuals();
    if (n > 1) {
        std::cout << "cos lib function must be called for one argument only\n";
        retval.type = nil_m;
        return;
    }
    retval.type = number_m;
    retval.data.numVal = std::cos((avm_getactual(0)->data.numVal));
}

void libfunc_sin(void)
{
    avm_memcellclear(&retval);
    unsigned n = avm_totalactuals();
    if (n > 1) {
        std::cout << "sqrt lib function must be called for one argument only\n";
        retval.type = nil_m;
        return;
    }
    retval.type = number_m;
    retval.data.numVal = std::sin((avm_getactual(0)->data.numVal));
}