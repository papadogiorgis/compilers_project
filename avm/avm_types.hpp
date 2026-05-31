#ifndef AVM_TYPES
#define AVM_TYPES

// <<<<<<< HEAD
#include <map>

// =======
// >>>>>>> 08e058ca58ede18473d46e41f1a2e86f5649862c
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

    std::map<double, struct avm_memcell*> numIndexed;
    std::map<std::string, struct avm_memcell*> strIndexed;
    std::map<struct avm_memcell*, struct avm_memcell*> otherIndexed;

    void decrrefcounter();
    avm_table(void);
    ~avm_table(void);
} avm_table;

typedef struct avm_memcell{
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
void avm_assign (avm_memcell *lv, avm_memcell *rv);
avm_memcell* avm_tablegetelem(avm_table* t, avm_memcell* index);
userfunc* avm_getfuncinfo(unsigned addr);
avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg);

#endif