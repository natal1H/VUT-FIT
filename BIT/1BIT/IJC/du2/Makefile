# Makefile for 2nd project for IJC
# Author: Natalia Holkova (xholko02), FIT
# Date: 20.3.2018

CFLAGS=-std=c99 -pedantic -Wall -Wextra -pedantic
CPPFLAGS=-std=c++11 -pedantic -Wall

CDYNFLAGS = -fPIC -ansi -Wall -pedantic

all: tail tail2 wordcount wordcount-dynamic

tail: tail.c tail.h
	gcc ${CFLAGS} $^ -o $@

tail2: tail2.cc
	g++ ${CPPFLAGS} $^ -o $@

htab_hash_function.o: htab_hash_function.c
	gcc ${CFLAGS} -c htab_hash_function.c

htab_init.o: htab_init.c
	gcc ${CFLAGS} -c htab_init.c

htab_size.o: htab_size.c
	gcc ${CFLAGS} -c htab_size.c

htab_bucket_count.o: htab_bucket_count.c
	gcc ${CFLAGS} -c htab_bucket_count.c

htab_find.o: htab_find.c
	gcc ${CFLAGS} -c htab_find.c

htab_lookup_add.o: htab_lookup_add.c
	gcc ${CFLAGS} -c htab_lookup_add.c

htab_remove.o: htab_remove.c
	gcc ${CFLAGS} -c htab_remove.c

htab_clear.o: htab_clear.c
	gcc ${CFLAGS} -c htab_clear.c

htab_free.o: htab_free.c
	gcc ${CFLAGS} -c htab_free.c

htab_move.o: htab_move.c
	gcc ${CFLAGS} -c htab_move.c

htab_foreach.o: htab_foreach.c
	gcc ${CFLAGS} -c htab_foreach.c


htab_hash_function_shared.o: htab_hash_function.c
	gcc -fPIC ${CFLAGS} -c htab_hash_function.c -o htab_hash_function_shared.o

htab_init_shared.o: htab_init.c
	gcc -fPIC ${CFLAGS} -c htab_init.c -o htab_init_shared.o

htab_size_shared.o: htab_size.c
	gcc -fPIC ${CFLAGS} -c htab_size.c -o htab_size_shared.o

htab_bucket_count_shared.o: htab_bucket_count.c
	gcc -fPIC ${CFLAGS} -c htab_bucket_count.c -o htab_bucket_count_shared.o

htab_find_shared.o: htab_find.c
	gcc -fPIC ${CFLAGS} -c htab_find.c -o htab_find_shared.o

htab_lookup_add_shared.o: htab_lookup_add.c
	gcc -fPIC ${CFLAGS} -c htab_lookup_add.c -o htab_lookup_add_shared.o

htab_remove_shared.o: htab_remove.c
	gcc -fPIC ${CFLAGS} -c htab_remove.c -o htab_remove_shared.o

htab_clear_shared.o: htab_clear.c
	gcc -fPIC ${CFLAGS} -c htab_clear.c -o htab_clear_shared.o

htab_free_shared.o: htab_free.c
	gcc -fPIC ${CFLAGS} -c htab_free.c -o htab_free_shared.o

htab_move_shared.o: htab_move.c
	gcc -fPIC ${CFLAGS} -c htab_move.c -o htab_move_shared.o

htab_foreach_shared.o: htab_foreach.c
	gcc -fPIC ${CFLAGS} -c htab_foreach.c -o htab_foreach_shared.o


libhtab.a: htab_hash_function.o htab_init.o htab_size.o htab_bucket_count.o htab_find.o htab_lookup_add.o htab_remove.o htab_clear.o htab_free.o htab_move.o htab_foreach.o
	ar rcs $@ $^

libhtab.so: htab_hash_function_shared.o htab_init_shared.o htab_size_shared.o htab_bucket_count_shared.o htab_find_shared.o htab_lookup_add_shared.o htab_remove_shared.o htab_clear_shared.o htab_free_shared.o htab_move_shared.o htab_foreach_shared.o
	gcc -fPIC ${CFLAGS} -shared $^ -o $@

wordcount: wordcount.c io.h io.c htab.h libhtab.a
	gcc ${CFLAGS} -static $^ -o $@

wordcount-dynamic: wordcount.c io.h io.c htab.h libhtab.so
	gcc -fPIC ${CFLAGS} $^ -o $@

zip: tail.c tail2.cc htab_hash_function.c htab_init.c htab_size.c htab_bucket_count.c htab_find.c htab_lookup_add.c htab_remove.c htab_clear.c htab_free.c htab_move.c htab_foreach.c wordcount.c io.c io.h htab.h Makefile
	zip xholko02.zip *.c *.cc *.h Makefile

clean:
	rm -f tail tail2 wordcount wordcount-dynamic libhtab.a libhtab.so *.o xholko02.zip
