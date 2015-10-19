#ifndef __MEMORY_H__
#define __MEMORY_H__

extern volatile void *main_memory;

void memory_init(void);
void *alloc_stack(void);

#endif
