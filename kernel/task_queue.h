#ifndef __K_TASK_QUEUE_H__
#define __K_TASK_QUEUE_H__

#include <stddef.h>

#include "task.h"

struct task_queue_node {
  struct task;
  struct task_queue_node *next;
  struct task_queue_node *prev;
};

struct task_queue {
  struct task_queue_node *first;
  struct task_queue_node *last;
};

struct priority_task_queue {
  struct task_queue priorities[NUM_PRIORITIES];
};

struct stateful_priority_task_queue {
  struct priority_task_queue states[NUM_TASK_STATES];
};

/** @brief Enqueue task node onto a stateful priority task queue
 *
 *  Running time: O(1)
 *
 *  @param sptq The queue to place the node onto
 *  @param node The node to place on the queue
 */
void EnqueueStatefulPriorityTaskQueue(
    struct stateful_priority_task_queue *sptq,
    struct task_queue_node *node
);

/** @brief Enqueue task node onto a task queue
 *
 *  Running time: O(1)
 *
 *  @param queue The queue to place the node onto
 *  @param node The node to place on the queue
 */
void EnqueueTaskQueue( struct task_queue *queue, struct task_queue_node *node);

/** @brief Dequeue task node from a priority task queue
 *
 *  Running time: O(1)
 *
 *  @param pq The queue to dequeue from
 *  @return Dequeued task node
 */
struct task_queue_node *DequeuePriorityTaskQueue( struct priority_task_queue *pq );

/** @brief Dequeue task node from a task queue
 *
 *  Running time: O(1)
 *
 *  @param task_queue The task queue to dequeue from
 *  @return Dequeued task node
 */
struct task_queue_node *DequeueTaskQueue( struct task_queue *queue );

/** @brief Remove a node from a stateful priority queue
 *
 *  Running time: O(1)
 *
 *  @param queue The task queue to remove the node from
 *  @param node The node to remove
 */
void RemoveStatefulPriorityTaskQueueNode(
    struct stateful_priority_task_queue *queue,
    struct task_queue_node *node
);

#endif
