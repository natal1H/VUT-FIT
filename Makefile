# Makefile for 2nd project for IJC
# Author: Natalia Holkova (xholko02), FIT
# Date: 20.3.2018

CFLAGS=-std=c99 -pedantic -Wall -Wextra -pedantic
CPPFLAGS=-std=c++11 -pedantic -Wall

all: tail tail2

tail: tail.c tail.h
	gcc ${CFLAGS} tail.c -o tail

tail2: tail2.cc
	g++ ${CPPFLAGS} tail2.cc -o tail2

clean:
	rm -f tail tail2
