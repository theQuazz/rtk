#include "queue.h"

void EnqueueQueue( struct queue *queue, struct queue_node *node) {
  if ( ! queue->last ) {
    queue->last = node;
  }

  if ( queue->first ) {
    queue->first->prev = node;
  }

  node->next = queue->first;
  node->prev = NULL;
  queue->first = node;
}

struct queue_node *DequeueQueue( struct queue *queue ) {
  struct queue_node *dequeued = queue->last;

  if ( ! dequeued ) {
    return NULL;
  }

  if ( dequeued->prev ) {
    dequeued->prev->next = NULL;
  }

  if ( queue->first == dequeued ) {
    queue->first = NULL;
  }

  queue->last = dequeued->prev;

  dequeued->next = dequeued->prev = NULL;

  return dequeued;
}

