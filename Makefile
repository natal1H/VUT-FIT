SOURCES = main.c tcpIPv4.c udpIPv4.c commonIPv4.c tcpIPv6.c

all: build

build:
	gcc $(SOURCES) -o ipk-scan -lpcap
