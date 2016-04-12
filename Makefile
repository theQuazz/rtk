DEBUG := NODEBUG
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

CC=arm-none-eabi-gcc
CFLAGS=-std=gnu99 -MMD -ffreestanding -O2 -Wall -Wextra -mcpu=arm1176jzf-s -fpic -marm -fplan9-extensions -D$(DEBUG)
LD=arm-none-eabi-gcc
LDFLAGS=-N -Ttext=0x10000 -nostartfiles -nostdlib -L$(HOME)/arm-cs-tools/lib/gcc/arm-none-eabi/4.8.3 -lgcc

SOURCES=$(wildcard ./kernel/*.c) $(wildcard ./arch/arm/*.c) $(wildcard ./lib/*.c) $(wildcard ./usr/*.c) main.c
ASM=$(wildcard ./arch/arm/*.s)
OBJECTS=$(SOURCES:.c=.o) $(ASM:.s=.o)
TESTSRCS=$(wildcard test/*.c)
TESTOBJS=$(TESTSRCS:.c=.o)
DEPENDS=$(SOURCES:.c=.d) $(TESTSRCS:.c=.d)

ELF=rtk.elf
BIN=rtk.bin
TEST_EXECUTABLE=runtests

#############################################################

.PHONY: build debug clean test

build: $(SOURCES) $(ELF)

debug: DEBUG=DEBUG
debug: $(SOURCES) $(ELF)

$(ELF): $(OBJECTS)

$(BIN): $(ELF)
	arm-none-eabi-objcopy $(ELF) -O binary $(BIN)

$(TEST_EXECUTABLE): $(OBJECTS) $(TESTOBJS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(TESTOBJS) -o $@

.SUFFIXES: .o .elf

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.s.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(ELF):
	$(LD) $(LDFLAGS) -o $@ $^


test: CC=gcc
test: CFLAGS=-Wall -Wextra --std=c99 -MMD -DTEST -DDEBUG -g
test: $(TESTSRCS) $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

#############################################################

$(OBJECTS) : $(MAKEFILE_NAME)			# OPTIONAL : changes to this file => recompile

-include $(DEPENDS)

clean:
	$(RM) ~* $(OBJECTS) $(TESTOBJS) $(EXECUTABLE) $(DEPENDS) $(TEST_EXECUTABLE) $(ELF) $(BIN) $(ISO)

