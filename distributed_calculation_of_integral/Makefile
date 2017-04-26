CC = gcc
COMPILER_FLGS = -Werror
DYNAMIC_LIBS = -pthread -lm

all: computer distributor

computer.o: computer.c common_code.h
	$(CC) $(COMPILER_FLGS) -c $<
 
distributor.o: distributor.c common_code.h
	$(CC) $(COMPILER_FLGS) -c $<

computer: computer.o
	$(CC) -o $@ $^ $(DYNAMIC_LIBS)

distributor: distributor.o
	$(CC) -o $@ $^ $(DYNAMIC_LIBS)

clean:
	rm -f *.o
	rm -f computer
	rm -f distributor
