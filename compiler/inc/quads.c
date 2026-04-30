#include "quads.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define EXPAND_SIZE 1024
#define CURR_SIZE (total *sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE*sizeof(quad) + CURR_SIZE)

unsigned currQuad = 1, total = 1;
quad *quads = NULL;

void expand(void) {
    assert(total == currQuad);
    quad *p = (quad *)malloc(NEW_SIZE);
    if (quads){
        memcpy(p, quads, CURR_SIZE);
        free(quads);
    }
    quads = p;
    total += EXPAND_SIZE;
}

void emit (iopcode op, expr *arg1, expr *arg2, expr *result, unsigned label, unsigned line){
    if (currQuad == total) expand();
    quad *p = quads+currQuad++;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;
}