#ifndef _STACK_H
#define _STACK_H

typedef struct {
	unsigned size;
	unsigned* scopeoffset_buf;
	unsigned top; // always points to the next available stack addr to put obj
} stack_t;

typedef struct lc_stack_t {
	struct lc_stack_t* next;
	unsigned cnt;
} lc_stack_t;

stack_t* stack_create(void);

unsigned stack_pop(stack_t* stack);

int stack_push(stack_t* stack, unsigned scopeoffset);

unsigned pop_and_top(stack_t* stack);

void stack_destroy(stack_t* stack);

void push_loopcounter(void);

void pop_loopcounter(void);

#endif