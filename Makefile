SOURCES = main.c tcpIPv4.c

all: build

build:
	gcc $(SOURCES) -o ipk-scan -lpcap
