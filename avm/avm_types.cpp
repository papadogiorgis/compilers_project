#include "avm_types.hpp"
#include "avm_stack.hpp"
#include <stdio.h>
#include <cstdlib>
#include <string>
#include "instr.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cassert>

extern avm_memcell ax, bx, cx, retval;
extern avm_memcell stack[STACK_SZ];
extern unsigned ebp, esp;
extern double* numConsts;
extern char** stringConsts;
extern char** namedLibfuncs;
extern userfunc* userFuncs;
extern unsigned totalUserFuncs;
extern unsigned avm_totalactuals(void);
extern unsigned char executionFinished;

const char* typeStrings[] = {
    "number",
    "string",
    "bool",
    "table",
    "userfunc",
    "libfunc",
    "nil",
    "undef"
};

unsigned totalTables=0;

avm_table::avm_table(){
    refcnt=0;
    tableno= ++totalTables;
}

avm_table::~avm_table(){
    for(auto& pair : numIndexed){
        avm_memcellclear(pair.second);
        delete pair.second;
    }
    for(auto& pair : strIndexed){
        avm_memcellclear(pair.second);
        delete pair.second;
    }
    for(auto& pair : otherIndexed){
        avm_memcellclear(pair.second);
        delete pair.second;
    }
    numIndexed.clear();
    strIndexed.clear();
    otherIndexed.clear();
}

void avm_table::incrrefcounter(void){
    ++refcnt;
}

void avm_table::decrrefcounter(void){
    assert(refcnt>0);
    if(--refcnt == 0){
        delete this;
    }
}

void avm_tablesetelem(avm_table* t, avm_memcell* key, avm_memcell *elem)
{
    assert(t && key && elem);
    
    if(key->type == number_m) {
        double num = key->data.numVal;
        auto it = t->numIndexed.find(num);

        if (elem->type == nil_m) {
            if (it != t->numIndexed.end()) {
                avm_memcellclear((it->second));
                delete it->second;
                t->numIndexed.erase(it);
            }
        }
        else {
            if (it != t->numIndexed.end()) {
                avm_assign(it->second, elem);
            }
            else {
                avm_memcell* newcell = new avm_memcell();
                newcell->type = undef_m;
                avm_assign(newcell, elem);
                t->numIndexed[num] = newcell;
            }
        }
    }
    else if (key->type == string_m) {
        std::string strKey = key->data.strVal;
        auto it = t->strIndexed.find(strKey);

        if(elem->type == nil_m) {
            if (it != t->strIndexed.end()) {
                avm_memcellclear((it->second));
                delete it->second;
                t->strIndexed.erase(it);
            }
        }
        else {
            if (it != t->strIndexed.end()) {
                avm_assign(it->second, elem);
            }
            else {
                avm_memcell* newcell = new avm_memcell();
                newcell->type = undef_m;
                avm_assign(newcell, elem);
                t->strIndexed[strKey] = newcell;
            }
        }
    }
    else {
        //std::cout << "warning set elem avm\n";
        bool found = false;
        for(auto it=t->otherIndexed.begin(); it != t->otherIndexed.end(); ++it){
            avm_memcell* k = it->first;
            bool match = false;
            if(k->type == key->type){
                if(k->type == bool_m) match=(k->data.boolVal == key->data.boolVal);
                else if(k->type == userfunc_m) match=(k->data.funcVal == key->data.funcVal);
                else if(k->type == libfunc_m) match=(strcmp(k->data.libFuncVal, key->data.libFuncVal)==0);
                else if(k->type == table_m) match=(k->data.tableVal == key->data.tableVal);
            }
            if(match){
                if(elem->type == nil_m){
                    avm_memcellclear(it->second);
                    delete it->second;
                    avm_memcellclear(it->first);
                    delete it->first;
                    t->otherIndexed.erase(it);
                }else{
                    avm_assign(it->second, elem);
                }
                found=true;
                break;
            }
        }
        if((!found)&&(elem->type != nil_m)){
            avm_memcell* newkey = new avm_memcell();
            newkey->type = undef_m;
            avm_assign(newkey, key);
            avm_memcell* newcell = new avm_memcell();
            newcell->type = undef_m;
            avm_assign(newcell, elem);
            t->otherIndexed[newkey] = newcell;
        }
    }
}

avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index){
    if(index->type == number_m){
        auto temp = table->numIndexed.find(index->data.numVal);
        if(temp != table->numIndexed.end()){
            return temp->second;
        }
    }else if(index->type == string_m){
        auto temp = table->strIndexed.find(index->data.strVal);
        if(temp != table->strIndexed.end()){
            return temp->second;
        }
    }else{
        auto temp = table->otherIndexed.find(index);
        if(temp != table->otherIndexed.end()){
            return temp->second;
        }
    }
    return nullptr;
}

userfunc* avm_getfuncinfo(unsigned address){
    for(unsigned i=0; i<totalUserFuncs; ++i){
        if(userFuncs[i].address == address){
            return &userFuncs[i];
        }
    }
    return nullptr;
}

userfunc* avm_getfuncinfo_byindex(unsigned index){
    if(index<totalUserFuncs){
        return &userFuncs[index];
    }
    return nullptr;
}

avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg){
    switch (arg->type) {
        case global_a: return &stack[STACK_SZ - 1 - arg->val];
        // case local_a: return &stack[esp -1- arg->val];
        // case formal_a: return &stack[esp + ENV_SZ + 1 + arg->val];
        case local_a: return &stack[ebp - arg->val];
        case formal_a:
            if(arg->val >= avm_totalactuals()){
                fprintf(stderr, "Error: out of stack - ask argument which is not pushed in the arguments list\n");
                executionFinished=1;
                static avm_memcell dummy;
                dummy.type = undef_m;
                return &dummy;
            }
            return &stack[ebp + ENV_SZ + 1 + arg->val];
        case retval_a: return &retval;

        case number_a: {
            reg->type = number_m;
            reg->data.numVal = numConsts[arg->val];
            return reg;
        }
        case string_a: {
            reg->type = string_m;
            reg->data.strVal = strdup(stringConsts[arg->val]);
            return reg;
        }
        case bool_a: {
            reg->type = bool_m;
            reg->data.boolVal = arg->val;
            return reg;
        }
        case nil_a: reg->type = nil_m; return reg;
        case userfunc_a: {
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;
        }
        case libfunc_a: {
            reg->type = libfunc_m;
            reg->data.libFuncVal = strdup(namedLibfuncs[arg->val]);
            // reg->data.libFuncVal = namedLibfuncs[arg->val]  ;
            return reg;
        }
        default:
            return nullptr;
    }
}

void avm_assign (avm_memcell *lv, avm_memcell *rv)
{
    if (lv == rv) {return;}

    // same tables
    if (lv->type == table_m && rv->type == table_m && lv->data.tableVal == rv->data.tableVal){
        return;
    }

    if (rv->type == undef_m) {
        fprintf(stderr, "warning: assigning from undef content\n");
    }

    //avm_memcellclear(lv);
    //memcpy(lv, rv, sizeof(avm_memcell));
    avm_memcell temp;
    memcpy(&temp, rv, sizeof(avm_memcell));

    if (temp.type == string_m){
        temp.data.strVal = strdup(rv->data.strVal);
    }else if (temp.type == libfunc_m) {
        temp.data.libFuncVal = strdup(rv->data.libFuncVal);
    }else if (temp.type == table_m) {
        temp.data.tableVal->incrrefcounter();
    }

    avm_memcellclear(lv);
    memcpy(lv, &temp, sizeof(avm_memcell));

    return;
}
