SOURCES = main.c tcpIPv4.c udpIPv4.c commonIPv4.c

all: build

build:
	gcc -Wall -Werror $(SOURCES) -o ipk-scan -lpcap
