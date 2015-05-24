all: Draughts 

clean:
	-rm Draughts.o Draughts

Draughts: Draughts.o
	gcc -o Draughts Draughts.o -lm -std=c99 -pedantic-errors -g

Draughts.o: Draughts.h Draughts.c
	gcc -std=c99 -pedantic-errors -c -Wall -g -lm Draughts.c

