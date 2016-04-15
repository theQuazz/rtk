#include "task_queue.h"

void EnqueueStatefulPriorityTaskQueue(
    struct stateful_priority_task_queue *sptq,
    struct task *node
) {
  struct queue *queue = &( sptq->states[node->state].priorities[node->priority] );

  EnqueueQueue( queue, node );
}

struct task *DequeuePriorityTaskQueue( struct priority_task_queue *pq ) {
  for ( enum Priority priority = HIGHEST_PRIORITY; priority <= NULL_PRIORITY; priority++ ) {
    struct queue_node *dequeued = DequeueQueue( &( pq->priorities[priority] ) );
    if ( dequeued ) {
      return ( void* )dequeued;
    }
  }

  return NULL;
}

void RemoveStatefulPriorityTaskQueueNode(
    struct stateful_priority_task_queue *sptq,
    struct task *node
) {
  struct queue *queue = &( sptq->states[node->state].priorities[node->priority] );

  RemoveQueueNode( queue, node );
}
