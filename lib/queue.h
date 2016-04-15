#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stddef.h>

struct queue_node {
  struct queue_node *next;
  struct queue_node *prev;
};

struct queue {
  struct queue_node *first;
  struct queue_node *last;
};

/** @brief Dequeue node from a queue
 *
 *  Running time: O(1)
 *
 *  @param task_queue The queue to dequeue from
 *  @return Dequeued task node
 */
struct queue_node *DequeueQueue( struct queue *queue );

/** @brief Enqueue node onto a queue
 *
 *  Running time: O(1)
 *
 *  @param queue The queue to place the node onto
 *  @param node The node to place on the queue
 */
void EnqueueQueue( struct queue *queue, struct queue_node *node);

/** @brief Remove node from a queue
 *
 *  Running time: O(1)
 *
 *  @param queue The queue to remove the node from
 *  @param node The node to remove from the queue
 */
void RemoveQueueNode( struct queue *queue, struct queue_node *node );

#endif
