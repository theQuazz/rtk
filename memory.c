#include <stddef.h>
#include "memory.h"

#ifdef TEST
#include <stdlib.h>
#endif

char stacks[NUM_STACKS][STACK_SIZE];

void memory_init(void) {
}

void *alloc_stack() {
  static int curr_stack = 0;
  return stacks[curr_stack++];
}
