FLAGS = -Werror -Wall -pedantic

all: isaserver isaclient

.PHONY: isaserver isaclient clean

isaserver:
	gcc $(FLAGS) isaserver.c common.c -o isaserver

isaclient:
	gcc $(FLAGS) isaclient.c common.c -o isaclient

clean:
	rm -f isaserver isaclient