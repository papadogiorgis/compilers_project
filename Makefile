COMPILER_FLAGS = -g -Wall  -I. -I./inc

AVM_FLAGS = -g -Wall  -I. -I./lib

COMPILER_SOURCE_FILES =  compiler/al.c \
				compiler/scanner.c \
				compiler/syntax_parser.c \
				compiler/inc/tokens_list.c \
				compiler/inc/symtablehash.c \
				compiler/inc/quads.c \
				compiler/inc/inter_code.c \
				compiler/inc/stack.c \
				compiler/inc/vm_target_code.c

AVM_SOURCE_FILES = avm/main.cpp \
				avm/instr.cpp \
				avm/avm_types.cpp \
				avm/avm_stack.cpp \
				avm/avm_loader.cpp \
				avm/avm_dispatch.cpp \
				avm/lib/functions.cpp

temp = avm/main.cpp\
	avm/avm_loader.cpp

all: alpha_compiler alpha_vm

alpha_compiler: compiler/scanner.c compiler/syntax_parser.c
	@gcc $(COMPILER_FLAGS) $(COMPILER_SOURCE_FILES) -o alpha_compiler
	@echo Alpha Compiler is ready!

alpha_vm:
	@g++ $(AVM_FLAGS) $(AVM_SOURCE_FILES) -o alpha_vm
	@echo Alpha Virtual Machine is ready!

compiler/scanner.c compiler/scanner.h: compiler/scanner.l
	@echo Generating Lexer...
	@cd compiler && flex --outfile=scanner.c --header-file=scanner.h scanner.l

compiler/syntax_parser.c compiler/syntax_parser.h: compiler/syntax_parser.y
	@echo Generating Syntax Parser...
	@cd compiler && bison -d -v --yacc --output=syntax_parser.c syntax_parser.y

clean: 
	@rm -f compiler/scanner.c compiler/syntax_parser.c compiler/syntax_parser.output alpha_compiler compiler/all_quads.txt out.abc alpha_vm
	@echo All cleaned up!