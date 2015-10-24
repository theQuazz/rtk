#ifndef __MEMORY_H__
#define __MEMORY_H__

enum {
  STACK_SIZE = 0x400,
  NUM_STACKS = 100,
};

void memory_init(void);
void *alloc_stack(void);

#endif
