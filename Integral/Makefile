CC = gcc
LINKER_FLGS = -pthread -lm
COMPILER_FLGS = -Werror -O2

all: integral

integral: clean SimpsonMethod.o
	$(CC) -o $@ SimpsonMethod.o $(LINKER_FLGS)
 
SimpsonsMethod.o: SimpsonMethod.c
	$(CC) -c $(COMPILER_FLGS) $<

clean:
	rm -f integral
	rm -f *.o
