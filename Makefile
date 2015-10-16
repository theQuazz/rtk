ISO=rtk.iso
TEST_EXECUTABLE=runtests

CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-T linker.ld -ffreestanding -O2 -nostdlib -lgcc

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o) boot.o
TESTSRCS=$(wildcard test/*.c)
TESTOBJS=$(TESTSRCS:.c=.o)
DEPENDS=$(SOURCES:.c=.d) $(TESTSRCS:.c=.d)

build: $(SOURCES) $(ISO)

$(ISO): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(TEST_EXECUTABLE): $(OBJECTS) $(TESTOBJS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(TESTOBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: clean test

test: CC=gcc
test: CFLAGS=-Wall -Wextra -pedantic --std=c99 -MMD -DTEST -DDEBUG -g
test: $(TESTSRCS) $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

-include ${DEPENDS}

clean:
	$(RM) ~* $(OBJECTS) $(TESTOBJS) $(EXECUTABLE) $(TEST_EXECUTABLE)

