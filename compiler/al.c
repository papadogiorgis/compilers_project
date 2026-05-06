#include <stdio.h>
#include <stdlib.h>

#include "inc/stack.h"
#include "scanner.h"
#include "inc/tokens_list.h"
#include "syntax_parser.h"
#include "inc/symtable.h"
#include "inc/quads.h"

extern int yylex(void);
extern int yyparse(void);
extern FILE* yyin;
extern SymTable_T symtable;
extern stack_t* stack;

struct token_list token_list = {NULL};

int main(int argc, char **argv){
    fprintf(stderr, "--------------- Syntax Analysis ---------------\n");
    symtable = SymTable_new();
    stack = stack_create();

    putLibFunctions(symtable);
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

    int parse_res = yyparse();
    if(parse_res != 0){
        fprintf(stderr, "\nParsing failed!\n");
    }
    

    fclose(yyin);
    printScopeList();

    FILE* quad_txt = fopen("all_quads.txt", "w");
    if(!quad_txt){
        fprintf(stderr, "Couldn't open file all_quads.txt\n");
    }else{
        give_quads(quad_txt);
    }

    return parse_res;
}
