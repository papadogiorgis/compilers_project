#ifndef AVM_DISPATCH
#define AVM_DISPATCH

typedef double (*arithmetic_func_t)(double x, double y);
typedef bool (*cmp_func) (double, double);

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

#endif