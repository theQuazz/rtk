#include "queue.h"

void EnqueueQueue( struct queue *queue, struct queue_node *node) {
  if ( ! queue->first ) {
    queue->first = node;
  }

  if ( queue->last ) {
    queue->last->next = node;
  }

  node->prev = queue->last;
  node->next = NULL;
  queue->last = node;
}

struct queue_node *DequeueQueue( struct queue *queue ) {
  struct queue_node *dequeued = queue->first;

  if ( ! dequeued ) {
    return NULL;
  }

  if ( dequeued->next ) {
    dequeued->next->prev = NULL;
  }

  if ( queue->last == dequeued ) {
    queue->last = NULL;
  }

  queue->first = dequeued->next;

  dequeued->next = dequeued->prev = NULL;

  return dequeued;
}

void RemoveQueueNode( struct queue *queue, struct queue_node *node ) {
  if ( ( void* )( queue->first ) == ( void* )node ) {
    queue->first = node->next;
  }

  if ( ( void* )( queue->last ) == ( void* )node ) {
    queue->last = node->prev;
  }

  if ( node->next ) {
    node->next->prev = node->prev;
  }

  if ( node->prev ) {
    node->prev->next = node->next;
  }

  node->next = node->prev = NULL;
}
