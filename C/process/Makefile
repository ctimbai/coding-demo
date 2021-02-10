# Makefile for lps
CC=gcc
CFLAGS=-std=gnu11 -Wall -g
DFLAGS=-DMAX_THREAD_SUPPORT=256
LFLAGS=

DEP=process.h
SRC=lps.c process.c
PROGRAMS=lps

lps:  $(DEP) $(SRC)
	$(CC) $(CFLAGS) $(DFLAGS) $(LFLAGS) $(SRC) -o $(PROGRAMS)

install:
	cp ./lps /usr/local/bin/

clean:
	rm $(PROGRAMS)
