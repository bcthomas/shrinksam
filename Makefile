PROGRAM_NAME = shrinksam
VERSION = 0.1
CC = g++
CFLAGS = -g -Wall -pedantic -DVERSION=$(VERSION) -std=gnu++11
DEBUG = -g
OPT = -O
ARCHIVE = $(PROGRAM_NAME)_$(VERSION)
LDFLAGS = -lstdc++
SDIR = src

.PHONY: clean default build distclean dist debug

default: build

debug:
	$(MAKE) build "CFLAGS=-Wall -pedantic -g -DDEBUG"

clean:
	rm -rf *.o $(SDIR)/*.o $(SDIR)/*.gch ./$(PROGRAM_NAME)

distclean: clean
	rm -rf *.tar.gz

dist:
	tar -zcf $(ARCHIVE).tar.gz *.ch Makefile

#main program
build: $(PROGRAM_NAME).o process_stream.o split.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OPT) $? -o $(PROGRAM_NAME)

shrinksam.o: $(SDIR)/$(PROGRAM_NAME).cpp $(SDIR)/$(PROGRAM_NAME).h $(SDIR)/global.h
	$(CC) $(CFLAGS) $(OPT) $(LDFLAGS) -c $(SDIR)/$*.cpp

process_stream.o: $(SDIR)/process_stream.cpp $(SDIR)/process_stream.h $(SDIR)/split.h $(SDIR)/global.h $(SDIR)/sam.h
	$(CC) $(CFLAGS) $(OPT) $(LDFLAGS) -c $(SDIR)/$*.cpp

split.o: $(SDIR)/split.cpp $(SDIR)/split.h
	$(CC) $(CFLAGS) $(OPT) $(LDFLAGS) -c $(SDIR)/$*.cpp
