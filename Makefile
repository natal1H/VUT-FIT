CC=gcc

OBJ=isaMIB.o
TARGETS=isaMIB.so
TARNAME=xholko02.tar

CFLAGS=-I. `net-snmp-config --cflags`
BUILDLIBS=`net-snmp-config --libs`
BUILDAGENTLIBS=`net-snmp-config --agent-libs`

# shared library flags (assumes gcc)
DLFLAGS=-fPIC -shared

all: $(TARGETS)

isaMIB.so: isaMIB.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o isaMIB.o isaMIB.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o isaMIB.so isaMIB.o

clean:
	rm $(OBJ) $(TARGETS) $(TARNAME)

tar: isaMIB.c isaMIB.h ISA-MIB.txt manual.pdf README
	tar -cvf $(TARNAME) isaMIB.c isaMIB.h ISA-MIB.txt Makefile manual.pdf README 

