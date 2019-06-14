CC=gcc
CFLAGS=-I.
DEPS = main.h
export LDFLAGS=-lrt

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

isrlz: main.o load.o rlz.o interpolation.o suffix_tree.o measures.o
	$(CC) -o isrlz main.o rlz.o interpolation.o load.o suffix_tree.o measures.o -lm -lrt
