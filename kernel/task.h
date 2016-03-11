#ifndef __TASK_H__
#define __TASK_H__

#include <stddef.h>

enum TaskState {
  READY,
  ACTIVE,
  ZOMBIE,
  SEND_BLOCKED,
  RECEIVE_BLOCKED,
  REPLY_BLOCKED,
  EVENT_BLOCKED,
};

/** @struct task
 *  @brief Task descriptor
 *
 *  Each task has a task descriptor (TD), which is the most important data
 *  structure in the kernel. The TDs should be local to the kernel, that is, on
 *  the kernel stack, allocated when the kernel begins executing.
 *
 *  @var task::tid A task identifier unique to this instance of the task
 *  @var task::state The task's state
 *  @var task::priority The task's priority
 *  @var task::parent_tid The task identifier of the task’s parent
 *  @var task::sp A stack pointer pointing to this task’s private memory
 *  @var task::spsr This task's saved program status register
 *  @var task::return_value The task's return value, which is to be returned to
 *  the task when it next executes.
 */
typedef struct task {
  const int tid;
  enum TaskState state;
  int priority;
  const int parent_tid;
  void *sp;
  uint32_t spsr;
  int return_value;
} Task;

/** @brief Instantiate a task
 *
 *  Create allocates and initializes a task descriptor, using the given
 *  priority, and the given function pointer as a pointer to the entry point of
 *  executable code, essentially a function with no arguments and no return
 *  value. When Create returns the task descriptor has all the state needed to
 *  run the task, the task’s stack has been suitably initialized, and the task
 *  has been entered into its ready queue so that it will run the next time it
 *  is scheduled
 *
 *  @param priority The priority of the task to be created
 *  @param code Entry point of the task
 *  @return Tid of new task or CreateError
 */
int Create( int priority, void ( *code )( void ) );
enum CreateError {
  ERR_INVALID_PRIORITY = -1,
  ERR_UNAVAILABLE_DESCRIPTOR = -2,
};

/** @brief Find my task id
 *
 *  MyTid returns the task id of the calling task.
 *
 *  @return The positive integer task id of the task that calls it
 */
int MyTid( void );

/** @brief Find the task id of the task that created the running task.
 *
 *  MyParentTid returns the task id of the task that created the calling task.
 *
 *  This will be problematic only if the task has exited or been destroyed, in
 *  which case the return value is implementation-dependent.
 *
 *  @return Tid of parent or MyParentTidError
 */
int MyParentTid( void );
enum MyParentTidError {
  ERR_PARENT_DESTROYED = -1,
};

/** @brief Cease execution, remaining ready to run
 *
 *  Pass causes a task to stop executing. The task is moved to the end of its
 *  priority queue, and will resume executing when next scheduled.
 */
void Pass( void );

/** @brief Terminate execution forever
 *
 *  Exit causes a task to cease execution permanently. It is removed from all
 *  priority queues, send queues, receive queues and awaitEvent queues.
 *  Resources owned by the task, primarily its memory and task descriptor are
 *  not reclaimed.
 *
 *  Exit does not return. If a point occurs where all tasks have exited the
 *  kernel should return cleanly to RedBoot.
 */
void Exit( void );

#endif
