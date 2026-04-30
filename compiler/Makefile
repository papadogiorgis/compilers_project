FLAGS = -g -Wall -I. -I./inc
SOURCE_FILES =  al.c \
				scanner.c \
				syntax_parser.c \
				inc/tokens_list.c \
				inc/symtablehash.c \
				inc/quads.c \
				inc/inter_code.c


all: parse

parse: syntax_parser.c scanner.c
	gcc $(FLAGS) $(SOURCE_FILES) -o parse

syntax_parser.c: syntax_parser.y
	bison -d -v --yacc --output=syntax_parser.c syntax_parser.y

scanner.c: scanner.l syntax_parser.c
	flex --outfile=scanner.c --header-file=scanner.h scanner.l

clean: 
	rm -f scanner.c syntax_parser.c syntax_parser.output parse all_quads.txt