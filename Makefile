# Makefile pre 1. projekt na IJC
# Autor: Natalia Holkova (xholko02), FIT
# Datum: 20.3.2018

# poziadavky ladiacich informacii
OPT=-O2
CFLAGS=-std=c99 -Wall -Wextra -pedantic

all: primes primes-i steg-decode

# treba vopred nastavit ulimit -s velkost_v_KiB
run: primes primes-i
	time ./primes
	time ./primes-i

# pouzitie makier
primes: primes.c error.o eratosthenes.o bit_array.h eratosthenes.h
	gcc ${CFLAGS} ${OPT} error.o eratosthenes.o primes.c -o $@ -lm

# pouzitie inline funkcii
primes-i: primes.c error.o eratosthenes.o eratosthenes.h
	gcc ${CFLAGS} ${OPT} error.o eratosthenes.o primes.c -o $@ -DUSE_INLINE -lm

steg-decode: error.o eratosthenes.o ppm.o
	gcc ${CFLAGS} ${OPT} error.o eratosthenes.o ppm.o steg-decode.c -o $@ -lm

error.o: error.c error.h
	gcc ${CFLAGS} ${OPT} -c error.c

eratosthenes.o: eratosthenes.c eratosthenes.h
	gcc ${CFLAGS} ${OPT} -c eratosthenes.c

ppm.o: ppm.c ppm.h
	gcc ${CFLAGS} ${OPT} -c ppm.c

zip: bit_array.h error.h error.c eratosthenes.h eratosthenes.c primes.c ppm.h ppm.c steg-decode.c
	zip xholko02.zip *.c *.h Makefile

clean:
	rm -f primes primes-i steg-decode error.o eratosthenes.o ppm.o xholko02.zip
