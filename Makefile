EXECUTABLE=rtk
TEST_EXECUTABLE=runtests

CC=gcc
CFLAGS=-Wall -Wextra -pedantic --std=c99 -MMD

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TESTSRCS=$(wildcard test/*.c)
TESTOBJS=$(TESTSRCS:.c=.o)
DEPENDS=$(SOURCES:.c=.d) $(TESTSRCS:.c=.d)

build: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(TEST_EXECUTABLE): $(OBJECTS) $(TESTOBJS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(TESTOBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean test

test: CFLAGS += -DTEST -DDEBUG -g
test: $(TESTSRCS) $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

-include ${DEPENDS}

clean:
	$(RM) ~* $(OBJECTS) $(TESTOBJS) $(EXECUTABLE) $(TEST_EXECUTABLE)

