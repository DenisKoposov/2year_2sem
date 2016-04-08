CC = gcc
LINKER_FLGS = -pthread
COMPILER_FLGS = -Werror

all: integral

integral: clean SimpsonMethod.o
	$(CC) -o $@ SimpsonMethod.o $(LINKER_FLGS)
 
SimpsonsMethod.o: SimpsonMethod.c
	$(CC) -c $< $(LINKER_FLGS)

clean:
	rm -f integral
	rm -f *.o
