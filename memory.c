#include <stddef.h>
#include "memory.h"

#ifdef TEST
#include <stdlib.h>
#endif

enum {
  STACK_SIZE = 0x400,
  NUM_STACKS = 100,
};

char stacks[NUM_STACKS][STACK_SIZE];

volatile void *main_memory;

void memory_init(void) {
#ifdef TEST
  main_memory = malloc(sizeof(char) * 1024 * 1024 * 64); // 64 MB
#else
  main_memory = NULL;
#endif
}

void *alloc_stack() {
  static int curr_stack = 0;
  return stacks[curr_stack++];
}
