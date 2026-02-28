#include <stdio.h>
#include "scanner.h"
#include "inc/tokens_list.h"

struct token_list token_list = {NULL};

int main(int argc, char **argv){
    if (argc > 1){
        if (!(yyin = fopen(argv[1], "r"))){
            fprintf(stderr, "Can't open file\n");
            return -1;
        }
    }

    alpha_token_t t;
    while (alpha_yylex(&t));
    fprintf(stderr, "--------------- Lexical Analysis ---------------\n");
    print_list(&token_list);
    return 0;
}
