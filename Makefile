SOURCES = main.c tcpIPv4.c

all: build

build:
	g++ $(SOURCES) -o ipk-scan -lpcap
