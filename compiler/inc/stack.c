/**
 * Stack implenentation for storing scope offsets,
 * used for intermediate code generation of functions.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

stack_t* stack_create(void)
{
	stack_t* new_stack = (stack_t*)malloc(sizeof(stack_t));
	if (!new_stack) {
		fprintf(stderr, "Malloc fail, for stack creation. Exiting\n");
		exit(-1);
	}

	new_stack->top = 0;
	new_stack->size = 2048;
	new_stack->scopeoffset_buf = malloc(sizeof(unsigned) * new_stack->size);
	if (!(new_stack->scopeoffset_buf)) {
		fprintf(stderr, "Malloc fail, for stack creation. Exiting\n");
		exit(-1);
	}

	return new_stack;
}

unsigned stack_pop(stack_t* stack)
{
	assert(stack->top != 0); // pre condition for calling pop(), is stack is not empty
    return stack->scopeoffset_buf[--stack->top];
}

int stack_push(stack_t* stack, unsigned scopeoffset)
{
	if (stack->top == stack->size) {
		stack->size = stack->size * 2;
		unsigned* tmp = (unsigned*)realloc(stack->scopeoffset_buf, sizeof(unsigned) * stack->size);
		if (!tmp) {
			fprintf(stderr, "Realloc for stack failed. Exit\n");
			exit(-1);
		}
		stack->scopeoffset_buf = tmp;
	}

	stack->scopeoffset_buf[stack->top++] = scopeoffset;
	return 1;
}

unsigned pop_and_top(stack_t *stack)
{
    stack_pop(stack);
    return(stack->top);
}

void stack_destroy(stack_t* stack)
{
	assert(stack);
	free(stack->scopeoffset_buf);
	free(stack);
}
