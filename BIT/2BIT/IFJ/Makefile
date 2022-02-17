CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -lm
SOURCES = main.c parser.c scanner.c stringlib.c expression_parser.c symtable.c semantic_analysis.c code_gen.c instr_list.c # Všetky .c zdrojové kódy
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

zip:
	zip xholko02.zip *.c *.h Makefile rozdeleni dokumentace.pdf

clean:
	rm -f $(EXECUTABLE)
