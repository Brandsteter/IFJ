CC = gcc
CFLAGS= -std=c99 -Wall -Wextra -Werror -pedantic -g

EXEC = ifj22_compiler
OBJS = syntax_analysis.o lexical_analysis.o expression_pars.o param_list.o par_stack.o symtable.o dynamic_string.o expression_stack.o code_generator.o undef_stack.o

$(EXEC): $(OBJS) 
	$(CC) $(OBJS) -o $(EXEC) 

.PHONY: clean

clean: 
	rm $(OBJS) $(EXEC)