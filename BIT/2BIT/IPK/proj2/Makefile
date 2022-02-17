SOURCES = main.c tcpIPv4.c udpIPv4.c commonIPv4.c tcpIPv6.c udpIPv6.c commonIPv6.c
HEADERS = main.h tcpIPv4.h udpIPv4.h commonIPv4.h tcpIPv6.h udpIPv6.h commonIPv6.h
DOCS = manual.pdf README
FLAGS = -Werror -Wall -pedantic

all: build

build:
	gcc $(FLAGS) $(SOURCES) -o ipk-scan -lpcap

tar:
	tar -cf xholko02.tar $(SOURCES) $(HEADERS) $(DOCS)
