FLAGS = -Werror -Wall -pedantic

all: isaserver isaclient

.PHONY: isaserver isaclient clean

isaserver:
	gcc $(FLAGS) isaserver.c common.c -o isaserver

isaclient:
	gcc $(FLAGS) isaclient.c common.c -o isaclient

tar:
	tar -cvf xholko02.tar isaclient.c isaclient.h isaserver.c isaserver.h common.c common.h Makefile manual.pdf README

clean:
	rm -f isaserver isaclient