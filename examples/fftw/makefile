CC=gcc

LIBS=-lfftw3 -lm

.PHONY: all clean

TARGETS=main

all: $(TARGETS)

clean: rm -f main.o

$(TARGETS): % : %.c ../../src/include/Ensen.h
	$(CC) $@.c -o $@ $(LIBS)
