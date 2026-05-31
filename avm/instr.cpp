#include "avm_stack.hpp"
#include "avm_types.hpp"
#include "instr.hpp"
#include <cassert>
#include <cstdlib>

extern void avm_assign (avm_memcell *lv, avm_memcell *rv);
extern avm_memcell ax, bx, cx, retval;
extern avm_memcell *stack;
extern unsigned esp, ebp, pc;
extern unsigned char executionFinished;
memclear_func_t memclearFuncs[]{
    0, //number
    memclear_string,
    0, //bool
    memclear_table,
    0, //userfunc
    0, //libfunc
    0, //nil
    0 //undef
};

extern void memclear_string (avm_memcell *m)
{
    assert(m->data.strVal);
    free(m->data.strVal);
}

extern void memclear_table (avm_memcell *m)
{
    assert(m->data.tableVal);
    // avm_tabledecrecounter(m->data.tableVal);
}


void avm_memcellclear (avm_memcell *m)
{
    if (m->type != undef_m){
        memclear_func_t f = memclearFuncs[m->type]; // find specific func to use to memclear
        if (f) {
            (*f) (m);
        }
        m->type = undef_m; // to avoid dereferencing m later as used
    }
}

