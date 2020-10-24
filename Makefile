# VUT FIT - IPK - Project No. 1

EXECUTABLE=client.py

all: run

run:
	@python3.6 $(EXECUTABLE) $(api_key) "$(city)"

build:

pack:
	zip xholko02.zip client.py Makefile README.md

clean:
	rm -f xholko02.zip

help:
	@python3.6 $(EXECUTABLE) --help

