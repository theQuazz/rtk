ELF=rtk.elf
BIN=rtk.bin
TEST_EXECUTABLE=runtests

CC=arm-none-eabi-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -mcpu=arm1176jzf-s -fpic -marm
LD=arm-none-eabi-gcc
LDFLAGS=-N -Ttext=0x10000 -nostartfiles -nostdlib -L$(HOME)/opt/cross/lib/gcc/arm-none-eabi/5.2.0 -lgcc

SOURCES=$(wildcard *.c)
ASM=$(wildcard *.s)
OBJECTS=$(SOURCES:.c=.o) $(ASM:.s=.o)
TESTSRCS=$(wildcard test/*.c)
TESTOBJS=$(TESTSRCS:.c=.o)
DEPENDS=$(SOURCES:.c=.d) $(TESTSRCS:.c=.d)

build: $(SOURCES) $(ELF)

$(ELF): $(OBJECTS)

$(BIN): $(ELF)
	arm-none-eabi-objcopy $(ELF) -O binary $(BIN)

$(TEST_EXECUTABLE): $(OBJECTS) $(TESTOBJS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(TESTOBJS) -o $@

.SUFFIXES: .o .elf

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^

.o.elf:
	$(LD) $(LDFLAGS) -o $@ $^

.PHONY: clean test

test: CC=gcc
test: CFLAGS=-Wall -Wextra -pedantic --std=c99 -MMD -DTEST -DDEBUG -g
test: $(TESTSRCS) $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

-include ${DEPENDS}

clean:
	$(RM) ~* $(OBJECTS) $(TESTOBJS) $(EXECUTABLE) $(TEST_EXECUTABLE)

