#include <stddef.h>
#include "memory.h"

#ifdef TEST
#include <stdlib.h>
#endif

volatile void *main_memory;

void memory_init(void) {
#ifdef TEST
  main_memory = malloc(sizeof(char) * 1024 * 1024 * 64); // 64 MB
#else
  main_memory = NULL;
#endif
}
