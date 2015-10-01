#include <stdio.h>
#include <stdlib.h>

#include "min_heap.h"

int sign(const int x) {
  return (x > 0) - (x < 0);
}

enum Comparison compare_ints(const void *a, const void *b) {
  return sign(a - b);
}

int main(void) {
  const int num_elements = 50;
  MinHeap queue = MinHeap_create(num_elements, compare_ints, malloc);
  int removed = 0;
  int num = 0;
  
  for (int i = 0; i < num_elements; i++) {
    num = rand() % 100;
    printf("inserting %d\n", num);
    MinHeap_insert(queue, (void *)num);
  }

  printf("%d\n", MinHeap_size(queue));

  for (int i = 0; i < num_elements; i++) {
    printf("%2d ", (int)MinHeap_delete_min(queue));
  }

  printf("\n");
}
