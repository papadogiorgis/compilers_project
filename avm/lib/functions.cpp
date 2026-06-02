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
extern avm_memcell stack[STACK_SZ];
extern unsigned ebp;
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

void libfunc_input(void){
    avm_memcellclear(&retval);
    std::string inputstr;
    if(!std::getline(std::cin, inputstr)){
        retval.type = nil_m;
        return;
    }

    if(inputstr == "true"){
        retval.type = bool_m;
        retval.data.boolVal = 1;
    }else if(inputstr == "false"){
        retval.type = bool_m;
        retval.data.boolVal = 0;
    }else if(inputstr == "nil"){
        retval.type = nil_m;
    }else{
        char* endptr;
        double num = strtod(inputstr.c_str(), &endptr);
        /*if the str has a valid num*/
        if((endptr != inputstr.c_str())&&(*endptr == '\0')){
            retval.type = number_m;
            retval.data.numVal = num;
        }else{
            /*if its a string*/
            retval.type = string_m;
            retval.data.strVal = strdup(inputstr.c_str());
        }
    }
}

void libfunc_objectmemberkeys(void){
    unsigned n = avm_totalactuals();
    if(n != 1){
        printf("Error: objectmemberkeys expects 1 argument\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }
    avm_memcell* arg = avm_getactual(0);
    if(arg->type != table_m){
        printf("Error: objectmemberkeys argument is not a table\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }

    avm_memcellclear(&retval);
    retval.type = table_m;
    retval.data.tableVal = new avm_table();
    retval.data.tableVal->incrrefcounter();
    avm_table* src = arg->data.tableVal;
    double i = 0.0;
    
    for(auto const& pair: src->numIndexed){
        avm_memcell index;
        index.type = number_m;
        index.data.numVal = i++;
        avm_memcell key;
        key.type = number_m;
        key.data.numVal = pair.first;
        avm_tablesetelem(retval.data.tableVal, &index, &key);
    }
    for(auto const& pair: src->strIndexed){
        avm_memcell index;
        index.type = number_m;
        index.data.numVal = i++;
        avm_memcell key;
        key.type = string_m;
        key.data.strVal = (char*)pair.first.c_str();
        avm_tablesetelem(retval.data.tableVal, &index, &key);
    }
    for(auto const& pair: src->otherIndexed){
        avm_memcell index;
        index.type = number_m;
        index.data.numVal = i++;
        avm_tablesetelem(retval.data.tableVal, &index, pair.first);
    }
}

void libfunc_objecttotalmembers(void){
    unsigned n = avm_totalactuals();
    if(n != 1){
        printf("Error: objecttotalmembers expects 1 argument\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }
    avm_memcell* arg = avm_getactual(0);
    if(arg->type != table_m){
        printf("Error: objecttotalmembers argument is not a table\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }
    avm_memcellclear(&retval);
    retval.type = number_m;
    retval.data.numVal = arg->data.tableVal->numIndexed.size() + arg->data.tableVal->strIndexed.size() + arg->data.tableVal->otherIndexed.size();
}

void libfunc_objectcopy(void){
    unsigned n = avm_totalactuals();
    if(n != 1){
        printf("Error: objectcopy expects 1 argument\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }
    avm_memcell* arg = avm_getactual(0);
    if(arg->type != table_m){
        printf("Error: objectcopy argument is not a table\n");
        avm_memcellclear(&retval);
        retval.type = nil_m;
        return;
    }

    avm_memcellclear(&retval);
    retval.type = table_m;
    retval.data.tableVal = new avm_table();
    retval.data.tableVal->incrrefcounter();
    avm_table* src = arg->data.tableVal;
    for(auto const& pair : src->numIndexed){
        avm_memcell key;
        key.type = number_m;
        key.data.numVal = pair.first;
        avm_tablesetelem(retval.data.tableVal, &key, pair.second);
    }
    for(auto const& pair : src->strIndexed){
        avm_memcell key;
        key.type = string_m;
        key.data.strVal = (char*)pair.first.c_str();
        avm_tablesetelem(retval.data.tableVal, &key, pair.second);
    }
    for(auto const& pair : src->otherIndexed){
        avm_tablesetelem(retval.data.tableVal, pair.first, pair.second);
    }
}


extern unsigned ebp;
void libfunc_totalarguments(void)
{
    // unsigned n = avm_totalactuals();
    avm_memcellclear(&retval);
    retval.type = undef_m;

    unsigned p_topsp = avm_get_envvalue(ebp + AVM_SAVEDTOPSP_OFFSET);

    // to check global scope
    if (p_topsp == STACK_SZ - 1) {
        std::cout << "Runtime error: totalarguments called outside of function\n";
        retval.type = nil_m;
        return;
    }

    if(p_topsp == 0){
        //std::cout << "totalarguments called outside of function\n";
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
    //retval.type = undef_m;

    unsigned p_topsp = avm_get_envvalue(ebp + AVM_SAVEDTOPSP_OFFSET);

    // to check global scope
    if (p_topsp == STACK_SZ - 1) {
        std::cout << "Runtime error: libfunc argument called outside of function\n";
        retval.type = nil_m;
        return;
    }

    if (n != 1) {
        std::cout << "lib function 'argument' must be called with one argument only\n";
        retval.type = nil_m;
        return;
    }

    avm_memcell* arg = avm_getactual(0);
    if(arg->type != number_m){
        std::cout<<"lib function 'argument' expects a number\n";
        retval.type = nil_m;
        return;
    }

    unsigned ptopsp = avm_get_envvalue(ebp + AVM_SAVEDTOPSP_OFFSET);
    if(ptopsp == 0){
        retval.type = nil_m;
        return;
    }

    unsigned call_num_actuals = avm_get_envvalue(ptopsp + AVM_NUMACTUALS_OFFSET);
    unsigned index = arg->data.numVal;
    if(index >= call_num_actuals){
        std::cout<<"Error: argument index out of bounds.\n";
        retval.type = nil_m;
        return;
    }
    avm_memcell* caller_arg = &stack[ptopsp + AVM_NUMACTUALS_OFFSET + 1 + index];
    avm_assign(&retval, caller_arg);
}


void libfunc_strtonum(void)
{
    avm_memcellclear(&retval);
    unsigned n = avm_totalactuals();
    if(n != 1){
        printf("Error: strtonum expects 1 argument\n");
        retval.type = nil_m;
        return;
    }
    avm_memcell* arg = avm_getactual(0);
    if(arg->type != string_m){
        printf("Runtime-Error: Failed to convert parameter to number in strtonum\n"); // DEBUG PRINTS the can be removed
        retval.type = nil_m;
        return;
    }

    char* str = arg->data.strVal;
    char* endptr;
    double num = strtod(str, &endptr);

    if((endptr == str)||(*endptr != '\0')){
        printf("Runtime-Error: Failed to convert parameter to number in strtonum\n");
        retval.type = nil_m;
    }else{
        retval.type = number_m;
        retval.data.numVal = num;
    }
}


void libfunc_sqrt(void)
{
    avm_memcellclear(&retval);
    unsigned n = avm_totalactuals();
    if (n != 1) {
        std::cout << "sqrt lib function must be called for one argument only\n";
        retval.type = nil_m;
        return;
    }
    double val = avm_getactual(0)->data.numVal;
    if(val<0){
        retval.type = nil_m;
        return;
    }
    retval.type = number_m;
    retval.data.numVal = std::sqrt(val);
}

void libfunc_cos(void)
{
    avm_memcellclear(&retval);
    unsigned n = avm_totalactuals();
    if (n != 1) {
        std::cout << "cos lib function must be called for one argument only\n";
        retval.type = nil_m;
        return;
    }
    double val = avm_getactual(0)->data.numVal;
    retval.type = number_m;
    retval.data.numVal = std::cos(val * M_PI / 180.0);
}

void libfunc_sin(void)
{
    avm_memcellclear(&retval);
    unsigned n = avm_totalactuals();
    if (n != 1) {
        std::cout << "sin lib function must be called for one argument only\n";
        retval.type = nil_m;
        return;
    }
    double val = avm_getactual(0)->data.numVal;
    retval.type = number_m;
    retval.data.numVal = std::sin(val * M_PI / 180.0);
}