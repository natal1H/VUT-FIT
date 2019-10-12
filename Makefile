FLAGS = -Werror -Wall -pedantic

all: isaserver isaclient

.PHONY: isaserver isaclient clean

isaserver:
	gcc $(FLAGS) isaserver.c -o isaserver

isaclient:
	gcc $(FLAGS) isaclient.c -o isaclient

clean:
	rm -f isaserver isaclient