#include "avm_stack.hpp"
#include "avm_types.hpp"
#include "instr.hpp"
#include <cassert>
#include <cstdlib>

extern void avm_assign (avm_memcell *lv, avm_memcell *rv);
extern avm_memcell ax, bx, cx;

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

void execute_assign(instruction *instr) 
{
    avm_memcell *lv = avm_translate_operand(&instr->result, (avm_memcell*) 0);
    avm_memcell *rv = avm_translate_operand(&instr->arg1, &ax);

    // assert(lv && (&stack[]));

    avm_assign(lv, rv);
}
