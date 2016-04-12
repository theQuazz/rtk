#include "task_queue.h"

void EnqueueStatefulPriorityTaskQueue(
    struct stateful_priority_task_queue *sptq,
    struct task_queue_node *node
) {
  struct task_queue *queue = &( sptq->states[node->state].priorities[node->priority] );

  EnqueueTaskQueue( queue, node );
}

void EnqueueTaskQueue( struct task_queue *queue, struct task_queue_node *node) {
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

struct task_queue_node *DequeuePriorityTaskQueue( struct priority_task_queue *pq ) {
  for ( enum Priority priority = HIGHEST_PRIORITY; priority < NULL_PRIORITY; priority++ ) {
    struct task_queue_node *dequeued = DequeueTaskQueue( &( pq->priorities[priority] ) );
    if ( dequeued ) {
      return dequeued;
    }
  }

  return NULL;
}

struct task_queue_node *DequeueTaskQueue( struct task_queue *queue ) {
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

void RemoveStatefulPriorityTaskQueueNode(
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
