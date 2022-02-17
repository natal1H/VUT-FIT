CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LFLAGS=-lpthread -lrt

all: proj2

proj2: proj2.c
	gcc $(CFLAGS) proj2.c -o proj2 $(LFLAGS)

zip:
	zip proj2.zip *.c *.h Makefile

clean:
	rm -f proj2 proj2.zip
