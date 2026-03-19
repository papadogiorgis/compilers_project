#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "inc/tokens_list.h"
#include "syntax_parser.h"
#include "inc/symtable.h"

extern int yylex(void);
extern int yyparse(void);
extern FILE* yyin;

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

    int parse_res = yyparse();
    if(parse_res == 0){
        fprintf(stderr, "\nParsigng completed!\n");
    }else{
        fprintf(stderr, "\nParsing failed!\n");
    }

    fprintf(stderr, "--------------- Lexical Analysis ---------------\n");
    // print_list(&token_list);

    fclose(yyin);

    /* hashtable test */
    SymTable_T symtable = SymTable_new();
    putLibFunctions(symtable);
    printScopeList();

    return parse_res;
}
