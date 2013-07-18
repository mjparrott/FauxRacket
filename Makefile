#Compiler
CC = gcc
#Flags to pass to the compiler
CFLAGS = -std=c99 -O -Wall -c -g

all: CFLAGS += -DNDEBUG
Repl: CFLAGS += -DNDEBUG
tests: CFLAGS += -DNDEBUG

all: Repl

Repl: Repl.o FauxRacket.o Sexp.o Helper.o AssociationList.o
	$(CC) Repl.o FauxRacket.o Sexp.o Helper.o AssociationList.o -o Repl
	
debug: Repl.o FauxRacket.o Sexp.o Helper.o AssociationList.o
	$(CC) Repl.o FauxRacket.o Sexp.o Helper.o AssociationList.o -o ReplDebug
	
tests: FauxRacket.o Sexp.o Helper.o AssociationList.o
	$(CC) $(CFLAGS) Testing/Tests.c -I. -o Testing/Tests.o
	$(CC) Testing/Tests.o FauxRacket.o Sexp.o Helper.o AssociationList.o -o Testing/Tests -DNDEBUG

Repl.o: Repl.c
	$(CC) $(CFLAGS) Repl.c
	
FauxRacket.o: FauxRacket.c
	$(CC) $(CFLAGS) FauxRacket.c
	
Sexp.o: Sexp.c
	$(CC) $(CFLAGS) Sexp.c
	
Helper.o: Helper.c
	$(CC) $(CFLAGS) Helper.c
	
AssociationList.o: AssociationList.c
	$(CC) $(CFLAGS) AssociationList.c

clean:
	rm -rf *.o Repl
