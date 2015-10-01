#include <stddef.h>

#include "min_heap.h"

struct min_heap {
  CompareFunction comparator;
  int size;
  int max;
  void **binary_tree;
};

MinHeap MinHeap_create(const int max, CompareFunction comparator, MemoryAllocator allocator) {
  struct min_heap *heap = allocator(sizeof(struct min_heap));

  heap->size = 0;
  heap->max = max;
  heap->comparator = comparator;
  heap->binary_tree = allocator(max * sizeof(void *));

  return heap;
}

int MinHeap_size(MinHeap heap) {
  return heap->size;
}

int MinHeap_max_size(MinHeap heap) {
  return heap->max;
}

CompareFunction MinHeap_comparator(MinHeap heap) {
  return heap->comparator;
}

MemoryAllocator MinHeap_memory_allocator(MinHeap heap) {
  return heap->allocator;
}

bool MinHeap_insert(MinHeap heap, const void * element) {
  int curr_index   = heap->size;
  int parent_index = 0;
  void *curr       = NULL;
  void *parent     = NULL;

  if (heap->size == heap->max) {
    return false;
  }

  heap->binary_tree[heap->size++] = element;

  while (curr_index != 0) {
    parent_index = (curr_index - 1) / 2;
    parent       = heap->binary_tree[parent_index];
    curr         = heap->binary_tree[curr_index];

    if (heap->comparator(curr, parent) == LESS_THAN) {
      heap->binary_tree[parent_index] = curr;
      heap->binary_tree[curr_index]   = parent;
      curr_index                      = parent_index;
    } else {
      break;
    }
  }
  
  return true;
}

void *MinHeap_delete_min(MinHeap heap) {
  void *min            = heap->binary_tree[0];
  int curr_index       = 0;
  int left_child_index = 1;
  int min_index        = 0;
  void *curr           = NULL;
  void *left_child     = NULL;
  void *right_child    = NULL;
  void *min_child      = NULL;

  heap->binary_tree[0] = heap->binary_tree[--heap->size];
  heap->binary_tree[heap->size] = NULL;

  while (left_child_index < heap->size) {
    curr        = heap->binary_tree[curr_index];
    left_child  = heap->binary_tree[left_child_index];
    right_child = heap->binary_tree[left_child_index + 1];
    min_index   = left_child_index;
    min_child   = left_child;

    if (left_child_index + 1 < heap->size &&
        heap->comparator(right_child, left_child) == LESS_THAN) {
      min_index += 1;
      min_child = right_child;
    }

    if (heap->comparator(min_child, curr) == LESS_THAN) {
      heap->binary_tree[min_index]   = curr;
      heap->binary_tree[curr_index]  = min_child;
      curr_index                     = min_index;
      left_child_index               = min_index * 2 + 1;
    } else {
      break;
    }
  }

  return min;
}
