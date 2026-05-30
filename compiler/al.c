#include <stdio.h>
#include <stdlib.h>

#include "inc/stack.h"
#include "scanner.h"
#include "inc/tokens_list.h"
#include "syntax_parser.h"
#include "inc/symtable.h"
#include "inc/quads.h"
#include "inc/vm_target_code.h"

extern int yylex(void);
extern int yyparse(void);
extern FILE* yyin;
extern SymTable_T symtable;
extern stack_t* stack;
extern lc_stack_t *lcs_top, *lcs_bot;
extern int err_count;
int print_syntax;

struct token_list token_list = {NULL};

int main(int argc, char **argv){
    symtable = SymTable_new();
    stack = stack_create();
    push_loopcounter();
    print_syntax=0;
    int file_flag=1;

    putLibFunctions(symtable);
    if (argc > 1){
        for(int i=1; i<argc; i++){
            if(strcmp(argv[i], "-print-syntax")==0){
                print_syntax=1;
            }else{
                if (!(yyin = fopen(argv[i], "r"))){
                    fprintf(stderr, "Can't open file\n");
                    return -1;
                }else{
                    file_flag=0;
                }
            }
        }
        if(file_flag){
            fprintf(stderr, "Give input file\n");
            return -1;
        }
    }else {
        fprintf(stderr, "Give input file\n");
        return -1;
    }

    if(print_syntax){
        fprintf(stderr, "--------------- Syntax Analysis ---------------\n");
    }
    int parse_res = yyparse();
    if(parse_res != 0){
        fprintf(stderr, "\nParsing failed!\n");
    }
    

    fclose(yyin);
    if(print_syntax){
        printScopeList();
    }

    FILE* quad_txt = fopen("compiler/all_quads.txt", "w");
    if(!quad_txt){
        fprintf(stderr, "Couldn't open file all_quads.txt\n");
    }else{
        if(err_count == 0){
            give_quads(quad_txt);
            generate_loop();
            create_binary_file();
        }
    }

    return parse_res;
}