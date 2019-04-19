SOURCES = main.c tcpIPv4.c udpIPv4.c commonIPv4.c tcpIPv6.c udpIPv6.c commonIPv6.c
FLAGS = -Werror -Wall -pedantic

all: build

build:
	gcc $(FLAGS) $(SOURCES) -o ipk-scan -lpcap
