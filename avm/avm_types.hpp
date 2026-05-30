#ifndef AVM_TYPES
#define AVM_TYPES

#include <map>

#include "../compiler/inc/vm_target_code.h"

enum avm_memcell_t {
    number_m,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
};


typedef struct avm_table{
    unsigned refcnt;
    unsigned tableno;
    void incrrefcounter();
    void decrrefcounter();
    avm_table(void);
    ~avm_table(void);
} avm_table;

typedef struct {
    avm_memcell_t type;
    union {
        double numVal;
        char *strVal;
        unsigned char boolVal;
        avm_table *tableVal;
        unsigned funcVal;
        char *libFuncVal;
    } data;
} avm_memcell;

avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg);
void avm_assign (avm_memcell *lv, avm_memcell *rv);

#endif