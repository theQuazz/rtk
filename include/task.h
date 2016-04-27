#ifndef __TASK_H__
#define __TASK_H__

#include "task_priority.h"

enum {
  MAX_TASKS = 1024,
};

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
int Create( enum Priority priority, void ( *code )() );
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
  ERR_NO_PARENT = -2,
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

/** @brief Get current task's priority
 *
 *  GetMyPriority return the priority of the calling task
 *
 *  @return Priority of current task
 */
int GetMyPriority( void );

/** @brief Set current task's priority
 *
 *  SetMyPriority return the priority of the calling task
 *
 *  @param to Priority to set it to
 */
void SetMyPriority( enum Priority to );

/** @brief Get task priority
 *
 *  GetCurrentPriority return the priority of the calling task
 *
 *  @param tid Tid of task to get priority of
 *  @return GetPriorityError or priority of task belonging to tid
 */
int GetPriority( int tid );
enum GetPriorityError {
  ERR_GET_PRIORITY_IMPOSSIBLE_TID = -1,
  ERR_GET_PRIORITY_NONEXISTENT_TASK = -2,
};

/** @brief Set task priority
 *
 *  SetCurrentPriority return the priority of the calling task
 *
 *  @param tid Tid of task to get priority of
 *  @param to Priority to set it to
 *  @return SetPriorityError or priority of task belonging to tid
 */
int SetPriority( int tid, enum Priority to );
enum SetPriorityError {
  ERR_SET_PRIORITY_INVALID_PRIORITY = -1,
  ERR_SET_PRIORITY_IMPOSSIBLE_TID = -2,
  ERR_SET_PRIORITY_NONEXISTENT_TASK = -3,
};

/** @brief Destroy a task
 *
 *  @param tid The tid of the task to destroy
 *  @return DestroyError or RETURN_STATUS_OK
 */
int Destroy( const int tid );
enum DestroyError {
  ERR_DESTROY_IMPOSSIBLE_TID = -1,
  ERR_DESTROY_NONEXISTENT_TASK = -2,
  ERR_NOT_CHILD = -3,
};

/** @brief Get task stats
 *
 * @param container TaskStats container to put stats into
 */
struct TaskStats {
  unsigned int tid;
  enum Priority priority;
  unsigned int state;
  long num_activates;
  unsigned long allowed_user_time;
  unsigned long used_user_time;
};
void GetTaskStats( int tid, struct TaskStats *container );

/** @brief Get all tasks stats
 *
 * @param container TasksStats container to put stats into
 */

struct TasksStats {
  unsigned int num_tasks;
  unsigned int num_alive_tasks;
  struct TaskStats tasks[MAX_TASKS];
};
void GetTasksStats( struct TasksStats *container );

#endif
