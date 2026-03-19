#include <stdio.h>
#include "scanner.h"
#include "inc/tokens_list.h"
#include "syntax_parser.h"

extern int yylex(void);

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

    // while (yylex());
    // while (yyparse());
    yyparse();

    fprintf(stderr, "--------------- Lexical Analysis ---------------\n");
    print_list(&token_list);
    return 0;
}
