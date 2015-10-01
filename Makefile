EXECUTABLE=rtk

CC=gcc
CFLAGS=-c -Wall -Wextra -pedantic --std=c99

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TESTSRCS=$(wildcard test/*.c)
TESTOBJS=$(TESTSRCS:.c=.o)

build: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(TESTS): $(TESTOBJS)
	$(CC) $(LDFLAGS) $(TESTOBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean clean_vim test

test: $(TESTS)
	./$(TESTS)

clean:
	$(RM) -rf $(OBJECTS) $(EXECUTABLE)

clean_vim: 
	find . \( -name ".*.un~" -o -name ".*.sw*" \) -exec $(RM) -rf {} \;
