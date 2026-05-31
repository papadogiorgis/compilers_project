#ifndef AVM_TYPES
#define AVM_TYPES

#include <map>
#include <string>

#include "../compiler/inc/vm_target_code.h"

struct avm_memcell;

enum avm_memcell_t {
    number_m=0,
    string_m=1,
    bool_m=2,
    table_m=3,
    userfunc_m=4,
    libfunc_m=5,
    nil_m=6,
    undef_m=7
};


typedef struct avm_table{
    unsigned refcnt;
    unsigned tableno;

    std::map<double, avm_memcell*> numIndexed;
    std::map<std::string, avm_memcell*> strIndexed;
    std::map<avm_memcell*, avm_memcell*> otherIndexed;

    void incrrefcounter();
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

extern const char* typeStrings[];

avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg);
void avm_assign (avm_memcell *lv, avm_memcell *rv);
avm_memcell* avm_tablegetelem(avm_table* table, avm_memcell* index);
userfunc* avm_getfuncinfo(unsigned address);

#endif