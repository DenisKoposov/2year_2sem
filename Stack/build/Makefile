CC = gcc
COMPILER_FLGS = -Wall -Werror -fprofile-arcs -ftest-coverage

#Relative path to sources
R_PATH = ../

#Paths to look for sources and headers
vpath %.h $(R_PATH)/headers/
vpath %.c $(R_PATH)/sources/

INCL = -I $(R_PATH)/headers/ -I $(R_PATH)/sources/

all: stack

stack_functions.o: stack_functions.c stack_functions.h
	$(CC) $(COMPILER_FLGS) -c $< $(INCL)

stack_main.o: stack_main.c stack_functions.h
	$(CC) $(COMPILER_FLGS) -c $< $(INCL)

stack:	stack_functions.o stack_main.o
	$(CC) -o $@ $^ -lgcov

clean:
	rm -f *.o
	rm -f stack
	rm -f *.gcno
	rm -f *.gcda
	rm -f *.c.gcov

