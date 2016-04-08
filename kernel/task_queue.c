#include "task_queue.h"

void enqueue_stateful_priority_task_queue(
    struct stateful_priority_task_queue *sptq,
    struct task_queue_node *node
) {
  struct task_queue *queue = &( sptq->states[node->state].priorities[node->priority] );

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

struct task_queue_node *dequeue_priority_task_queue( struct priority_task_queue *pq ) {
  for ( enum Priority priority = HIGHEST_PRIORITY; priority < NULL_PRIORITY; priority++ ) {
    struct task_queue_node *dequeued = dequeue_task_queue( &( pq->priorities[priority] ) );
    if ( dequeued ) {
      return dequeued;
    }
  }

  return NULL;
}

struct task_queue_node *dequeue_task_queue( struct task_queue *queue ) {
  struct task_queue_node *dequeued = queue->last;

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

void remove_stateful_priority_task_queue_node(
    struct stateful_priority_task_queue *sptq,
    struct task_queue_node *node
) {
  struct task_queue *queue = &( sptq->states[node->state].priorities[node->priority] );

  if ( queue->first == node ) {
    queue->first = node->next;
  }

  if ( queue->last == node ) {
    queue->last = node->prev;
  }

  node->next->prev = node->prev;
  node->prev->next = node->next;
  node->next = node->prev = NULL;
}
