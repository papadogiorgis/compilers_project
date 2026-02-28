CC = gcc
FLAGS = -g -Wall -I.
FLEX = scanner.l
FLEX_C = scanner.c
TOKENS_C = inc/tokens_list.c
AL = al.c

all: $(FLEX_C) $(TOKENS_C)
	$(CC) $(FLAGS) $(AL) $(FLEX_C) $(TOKENS_C) -o parse

$(FLEX_C): $(FLEX)
	flex --outfile=$(FLEX_C) --header-file=scanner.h $(FLEX) 

clean: 
	rm -f $(FLEX_C) parse scanner.h