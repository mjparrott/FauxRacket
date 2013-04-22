#Compiler
CC = gcc
#Flags to pass to the compiler
CFLAGS = -std=c99 -O -Wall -c

all: Repl

Repl: Repl.o FauxRacket.o Sexp.o Helper.o
	$(CC) Repl.o FauxRacket.o Sexp.o Helper.o -o Repl

Repl.o: Repl.c
	$(CC) $(CFLAGS) Repl.c
	
FauxRacket.o: FauxRacket.c
	$(CC) $(CFLAGS) FauxRacket.c
	
Sexp.o: Sexp.c
	$(CC) $(CFLAGS) Sexp.c
	
Helper.o: Helper.c
	$(CC) $(CFLAGS) Helper.c

clean:
	rm -rf *.o Repl
