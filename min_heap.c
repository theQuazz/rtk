#include <stddef.h>

#include "min_heap.h"

MinHeap MinHeap__create(void *mem, size_t size, CompareFunction comparator) {
  MinHeap heap = mem;

  MinHeap_init(
    heap,
    (void *)((char *)mem + sizeof(*heap)),
    (size - sizeof(*heap)) / sizeof(*heap->binary_tree),
    comparator
  );

  return heap;
}

MinHeap MinHeap__allocate_and_create(const int max, CompareFunction comparator, MemoryAllocator allocator) {
  MinHeap heap;
  size_t size = max * sizeof(*heap->binary_tree) + sizeof(*heap);
  void *mem = allocator(size);
  heap = MinHeap__create(mem, size, comparator);

  return heap;
}

void MinHeap_init(MinHeap heap, void *tree_space, size_t length, CompareFunction comparator) {
  heap->size = 0;
  heap->max = length;
  heap->binary_tree = tree_space;
  heap->comparator = comparator;
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

bool MinHeap_insert(MinHeap heap, void *element) {
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

  if (heap->size == 0) return NULL;

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

#ifdef DEBUG
#include <stdio.h>

void MinHeap_inspect(MinHeap heap, void (*element_inspect_function)(void *element)) {
  printf("#<MinHeap:%p @size=%d @max=%d @binary_tree={ ", (void*)heap, heap->size, heap->max);
  for (int i = 0; i < heap->size; i++) {
    void *el = heap->binary_tree[i];
    printf("%d=>", i);
    element_inspect_function(el);
    if ( i < heap->size - 1) printf(", ");
  }
  printf(" }>");
}

#endif
