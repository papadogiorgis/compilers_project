#include "tokens_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int add_token(struct token_list *list, int lineno, int *tokenno, char *content, char *category, char *type){
    alpha_token_t *new_token = malloc(sizeof(alpha_token_t));
    (*tokenno)++;
    new_token->lineno = lineno;
    new_token->tokenno = *tokenno;
    new_token->content = strdup(content);
    new_token->category = strdup(category);
    new_token->type = strdup(type);
    new_token->next=NULL;

    if (list->head == NULL){
        list->head = new_token;
    }
    else {
        alpha_token_t *curr = list->head;
        while (curr->next != NULL){
            curr = curr->next;
        }
        curr->next = new_token;
    }
    return 1;
}

void print_list(struct token_list *list){
    alpha_token_t *curr = list->head;
    while (curr != NULL) {
        printf("lineno: %d, tokenno: %d, content: %s, category: %s, type: %s\n", curr->lineno, curr->tokenno, curr->content, curr->category, curr->type);
        curr = curr->next;
    }
    return;
}