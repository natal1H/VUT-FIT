all: isaserver isaclient

isaserver:
	gcc isaserver.c -o isaserver

isaclient:
	gcc isaclient.c -o isaclient

clean:
	rm -f isaserver isaclient