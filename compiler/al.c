#include <stdio.h>
#include "scanner.h"
#include "inc/tokens_list.h"

extern int alpha_yylex(void *tok);

struct token_list token_list = {NULL};

int main(int argc, char **argv){
    if (argc > 1){
        if (!(yyin = fopen(argv[1], "r"))){
            fprintf(stderr, "Can't open file\n");
            return -1;
        }
    }
    else {
        fprintf(stderr, "Give input file\n");
        return -1;
    }

    alpha_token_t *t = malloc(sizeof(alpha_token_t));
    while (alpha_yylex(t)){
        t = malloc(sizeof(alpha_token_t));
    }
    fprintf(stderr, "--------------- Lexical Analysis ---------------\n");
    print_list(&token_list);
    return 0;
}
