CC = gcc
FLAGS = -g -Wall -I.
FLEX = scanner.l
FLEX_C = scanner.c
SYNTAX = syntax_parser.y
SYNTAX_C = syntax_parser.c
TOKENS_C = inc/tokens_list.c
AL = al.c

all: $(FLEX_C) $(TOKENS_C) $(SYNTAX_C)
	$(CC) $(FLAGS) $(AL) $(FLEX_C) $(SYNTAX_C) $(TOKENS_C) -o parse

$(FLEX_C): $(FLEX)
	flex --outfile=$(FLEX_C) --header-file=scanner.h $(FLEX) 

$(SYNTAX_C): $(SYNTAX)
	bison --yacc --defines --output=$(SYNTAX_C) $(SYNTAX)

clean: 
	rm -f $(FLEX_C) $(SYNTAX_C) parse scanner.h syntax_parser.h