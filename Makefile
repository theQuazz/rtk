ELF=rtk.elf
BIN=rtk.bin
TEST_EXECUTABLE=runtests

CC=arm-none-eabi-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -mcpu=arm1176jzf-s -fpic -marm -fplan9-extensions
LD=arm-none-eabi-gcc
LDFLAGS=-N -Ttext=0x10000 -nostartfiles -nostdlib -L$(HOME)/arm-cs-tools/lib/gcc/arm-none-eabi/4.8.3 -lgcc

SOURCES=$(wildcard ./kernel/*.c) $(wildcard ./arch/arm/*.c) $(wildcard ./lib/*.c) $(wildcard ./usr/*.c) main.c
ASM=$(wildcard ./arch/arm/*.s)
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

rtk.elf:
	$(LD) $(LDFLAGS) -o $@ $^

.PHONY: clean test

test: CC=gcc
test: CFLAGS=-Wall -Wextra -pedantic --std=c99 -MMD -DTEST -DDEBUG -g
test: $(TESTSRCS) $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

-include ${DEPENDS}

clean:
	$(RM) ~* $(OBJECTS) $(TESTOBJS) $(EXECUTABLE) $(TEST_EXECUTABLE)

