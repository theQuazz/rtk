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

MinHeap MinHeap_create(const int max, CompareFunction comparator, MemoryAllocator allocator);
bool MinHeap_insert(MinHeap heap, const void * element);
void *MinHeap_delete_min(MinHeap heap);
int MinHeap_size(MinHeap heap);
int MinHeap_max_size(MinHeap heap);
CompareFunction MinHeap_comparator(MinHeap heap);
MemoryAllocator MinHeap_memory_allocator(MinHeap heap);

#endif
