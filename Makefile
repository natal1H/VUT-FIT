all: isaserver isaclient

isaserver:
	gcc -Wall -Werror isaserver.c -o isaserver

isaclient:
	gcc -Wall -Werror isaclient.c -o isaclient

clean:
	rm -f isaserver isaclient