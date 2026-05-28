#ifndef VM_TYPES_DEF
#define VM_TYPES_DEF

enum vmopcode {
    assign_v, add_v, sub_v, mul_v, div_v,
    mod_v, uminus_v, and_v, or_v, not_v, jeq_v,
    jne_v, jle_v, jge_v, jlt_v, jgt_b, call_v, 
    pusharg_v, funcenter_v, funcexit_v, newtable_v,
    tablegetelem_v, tablesetelem_v, nop_v,
};

enum vmarg_t{
    label_a,
    global_a,
    formal_a,
    local_a,
    number_a,
    string_a,
    bool_a,
    nil_a,
    userfunc_a,
    libfunc_a,
    retval_a,
};

typedef struct vmarg {
    vmarg_t type;
    unsigned val;
} vmarg ;

typedef struct instruction {
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg  arg2;
    unsigned srcline;
} instruction;

typedef struct userfunc {
    unsigned address;
    unsigned localsize;
    char *id;
} userfunc;

#endif