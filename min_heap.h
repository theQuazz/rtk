#ifndef __MIN_HEAP_H__
#define __MIN_HEAP_H__

#include <stdbool.h>

enum Comparison {
  LESS_THAN    = -1,
  EQUAL        = 0,
  GREATER_THAN = 1
};

typedef enum Comparison (*CompareFunction)(const void *, const void *);
typedef void *(*MemoryAllocator)(size_t size);

typedef struct min_heap *MinHeap; 

// STATIC
MinHeap MinHeap__allocate_and_create(const int max, CompareFunction comparator, MemoryAllocator allocator);
MinHeap MinHeap__create(void *mem, size_t size, CompareFunction comparator);

// INSTANCE
void MinHeap_init(MinHeap heap, void *tree_space, size_t length, CompareFunction comparator);
bool MinHeap_insert(MinHeap heap, void *element);
void *MinHeap_delete_min(MinHeap heap);
int MinHeap_size(MinHeap heap);
int MinHeap_max_size(MinHeap heap);
CompareFunction MinHeap_comparator(MinHeap heap);
MemoryAllocator MinHeap_memory_allocator(MinHeap heap);

#ifdef DEBUG
void MinHeap_inspect(MinHeap heap, void (*element_inspect_function)(void *element));
#else
#define MinHeap_inspect(...)
#endif

#endif
