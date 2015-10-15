#include <stdio.h>
#include <stdlib.h>

#include "../min_heap.h"
#include "test.h"

int sign(const int x) {
  return (x > 0) - (x < 0);
}

enum Comparison compare_longs(const void *a, const void *b) {
  return sign(*(long*)a - *(long*)b);
}

T_SUITE(MinHeap) {
  T_TEST("inserted elements come out in the right order") {
    const int num_elements = 50;
    long numbers[num_elements];
    long elements[num_elements];
    MinHeap queue = MinHeap__allocate_and_create(num_elements, compare_longs, malloc);

    for (int i = 0; i < num_elements; i++) {
      numbers[i] = rand() % 100;
      elements[i] = numbers[i];
      MinHeap_insert(queue, &numbers[i]);
    }

    T_ASSERT(MinHeap_size(queue) == num_elements);

    qsort(elements, num_elements, sizeof(*elements), compare_longs);

    for (int i = 0; i < num_elements; i++) {
      T_ASSERT(compare_longs(MinHeap_delete_min(queue), &elements[i]) == EQUAL);
    }
  }
}
