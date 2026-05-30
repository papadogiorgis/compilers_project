FLAGS = -g -Wall  -I. -I./inc
SOURCE_FILES =  compiler/al.c \
				compiler/scanner.c \
				compiler/syntax_parser.c \
				compiler/inc/tokens_list.c \
				compiler/inc/symtablehash.c \
				compiler/inc/quads.c \
				compiler/inc/inter_code.c \
				compiler/inc/stack.c \
				compiler/inc/vm_target_code.c


all: alpha_compiler #avm

alpha_compiler: compiler/scanner.c compiler/syntax_parser.c
	@gcc $(FLAGS) $(SOURCE_FILES) -o alpha_compiler
	@echo Alpha Compiler is ready!

#avm:

compiler/scanner.c compiler/scanner.h: compiler/scanner.l
	@echo Generating Lexer...
	@cd compiler && flex --outfile=scanner.c --header-file=scanner.h scanner.l

compiler/syntax_parser.c compiler/syntax_parser.h: compiler/syntax_parser.y
	@echo Generating Syntax Parser...
	@cd compiler && bison -d -v --yacc --output=syntax_parser.c syntax_parser.y

clean: 
	@rm -f compiler/scanner.c compiler/syntax_parser.c compiler/syntax_parser.output alpha_compiler compiler/all_quads.txt compiler/out.abc
	@echo All cleaned up!