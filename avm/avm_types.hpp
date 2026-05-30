#ifndef AVM_TYPES
#define AVM_TYPES

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

// TODO: define later
typedef tableval;
typedef avm_table;

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

typedef void (*execute_func_t)(instruction *);

avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg);

#endif