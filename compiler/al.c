#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "inc/tokens_list.h"
#include "syntax_parser.h"
#include "inc/symtable.h"

extern int yylex(void);
extern int yyparse(void);
extern FILE* yyin;
extern SymTable_T symtable;

struct token_list token_list = {NULL};

int main(int argc, char **argv){
    fprintf(stderr, "--------------- Syntax Analysis ---------------\n");
    symtable = SymTable_new();
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

    return parse_res;
}
