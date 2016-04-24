#include "task.h"
#include "task_queue.h"
#include "../include/common.h"
#include "../lib/string.h"
#include "../lib/debug.h"
#include "../lib/math.h"

#ifdef DEBUG
#define DebugInspectTask( task ) \
  DebugPrint( "#<Task:0x%08x tid=%d priority=%d user_time=%lu last_activate_at=%lu num_activates=%ld>", task, task->tid, task->priority, task->used_user_time / 1000, task->last_activate_at, task->num_activates );
#else
#define DebugInspectTask(...)
#endif

enum {
  NUM_REGS_POPPED = 14,
};

static volatile int num_tasks = 0;

static volatile int task_generation = -1;

static struct task task_descriptors[MAX_TASKS];

static struct task *current_task = NULL;

static struct stateful_priority_task_queue scheduler;
static struct queue available_descriptors = {
  .first = task_descriptors,
  .last = task_descriptors + MAX_TASKS - 1,
};

static uint32_t stacks[MAX_TASKS][STACK_SIZE_WORDS];

void InitTasks( void ) {
  for ( unsigned int i = 0; i < MAX_TASKS; i++ ) {
    task_descriptors[i].tid = i;
    task_descriptors[i].generation = task_generation;
    task_descriptors[i].stack = stacks[i];
    task_descriptors[i].state = FREE;
    EnqueueQueue( &available_descriptors, &task_descriptors[i] );
  }
}

int GetCurrentTid( void ) {
  return current_task ? current_task->tid : -1;
}

int GetParentTid( void ) {
  if ( ! current_task->parent ) {
    return ERR_NO_PARENT;
  }

  if ( current_task->parent->state == ZOMBIE ) {
    return ERR_PARENT_DESTROYED;
  }

  return current_task->parent->tid;
}

int GetCurrentTaskPriority( void ) {
  return current_task ? current_task->priority : -1;
}

int SetCurrentTaskPriority( enum Priority to ) {
  if ( ! current_task ) {
    return -1;
  }

  if ( LOW_PRIORITY < to ) {
    return ERR_INVALID_PRIORITY;
  }

  current_task->priority = to;

  return RETURN_STATUS_OK;
}

int GetTaskPriority( const int tid ) {
  Debugln( "Getting priority of (%d)", tid );

  if ( ! IsValidTid( tid ) ) {
    Debugln( "Invalid tid, aborting!" );
    return ERR_GET_PRIORITY_IMPOSSIBLE_TID;
  }

  if ( ! IsTaskAlive( tid ) ) {
    Debugln( "Non-existent tid, aborting!" );
    return ERR_GET_PRIORITY_NONEXISTENT_TASK;
  }

  return task_descriptors[tid].state;
}

int SetTaskPriority( const int tid, enum Priority to ) {
  Debugln( "Setting priority of (%d) to (%u)", tid, to );

  if ( ! IsValidPriority( to ) ) {
    Debugln( "Invalid priority, aborting!" );
    return ERR_SET_PRIORITY_INVALID_PRIORITY;
  }

  if ( ! IsValidTid( tid ) ) {
    Debugln( "Invalid tid, aborting!" );
    return ERR_SET_PRIORITY_IMPOSSIBLE_TID;
  }

  if ( ! IsTaskAlive( tid ) ) {
    Debugln( "Non-existent tid, aborting!" );
    return ERR_SET_PRIORITY_NONEXISTENT_TASK;
  }

  RemoveStatefulPriorityTaskQueueNode( &scheduler, &task_descriptors[tid] );
  task_descriptors[tid].priority = to;
  if ( tid != current_task->tid ) {
    EnqueueStatefulPriorityTaskQueue( &scheduler, &task_descriptors[tid] );
  }

  return RETURN_STATUS_OK;
}

void Nop( void ) {
  Debug( "Pass " );
  DebugInspectTask( current_task );
  DebugPrint( "\r\n" );
}

void ExitTask( void ) {
  num_tasks -= 1;

  current_task->state = ZOMBIE;

  Debug( "Exit (%d) from ", num_tasks );
  DebugInspectTask( current_task );
  DebugPrint( "\r\n" );
}

static struct task *GetNextScheduledTask( void ) {
  return DequeuePriorityTaskQueue( &scheduler.states[READY] );
}

static void ScheduleTask( struct task *task ) {
  EnqueueStatefulPriorityTaskQueue( &scheduler, task );
}

int CreateTask( int priority, void ( *code )( void ) ) {
  struct task *task = ( struct task* )DequeueQueue( &available_descriptors );
  if ( ! task ) {
    return ERR_UNAVAILABLE_DESCRIPTOR;
  }

  task->next = NULL;
  task->prev = NULL;
  task->generation = task->generation + 1;
  task->state = READY;
  task->priority = priority;
  task->parent = current_task;
  task->sp = task->stack + STACK_SIZE_WORDS - NUM_REGS_POPPED; // allow space for 14 registers (word size = 4 bytes) to be popped
  task->spsr = 0x10;
  task->pc = ( long )code;
  task->stack[STACK_SIZE_WORDS - 1] = ( uint32_t )Exit; // task initial lr
  task->awaiting_event_id = -1;
  task->used_user_time = 0;
  task->children.first = NULL;
  task->children.last = NULL;

  if ( current_task ) {
    EnqueueQueue( &current_task->children, &task->siblings );
  }

  ScheduleTask( task );

  num_tasks += 1;

  Debug( "Creating " );
  DebugInspectTask( task );
  DebugPrint( "\r\n" );

  return task->tid;
}

int CreateTaskSafe( int priority, void ( *code )( void ) ) {
  if ( priority < HIGHEST_PRIORITY || LOW_PRIORITY < priority ) {
    return ERR_INVALID_PRIORITY;
  }

  return CreateTask( priority, code );
}

void ScheduleAndActivate( void ) {
  if ( current_task->state == ACTIVE ) {
    current_task->state = READY;
  }

  ScheduleTask( current_task );
  current_task = GetNextScheduledTask();

  current_task->state = ACTIVE;
  current_task->last_activate_at = GetTimerTime();
  current_task->allowed_user_time += current_task->last_activate_at / 100;
  current_task->num_activates += 1;

  Debug( "Switching to " );
  DebugInspectTask( current_task );
  DebugPrint( "\r\n" );

  Activate( current_task->spsr, current_task->sp, current_task->pc );
}

void SaveTaskState( uint32_t spsr, uint32_t *sp, uint32_t pc, uint32_t timer ) {
	current_task->spsr = spsr;
	current_task->sp = sp;
	current_task->pc = pc;
  if ( TimerWentOff() ) {
    timer = 0;
  }
  current_task->used_user_time += ( current_task->last_activate_at - timer ) / 100;
}

void SetCurrentTaskReturnValue( uint32_t ret ) {
  *( current_task->sp ) = ret; // TODO: find better way of storing return code?
}

void SetCurrentTaskOversizedReturnValue( void *ret, size_t size ) {
  memcpy( current_task->sp + NUM_REGS_POPPED, ret, size );
}

void SetTaskReturnValue( const int tid, uint32_t ret ) {
  *( task_descriptors[tid].sp ) = ret; // TODO: find better way of storing return code?
}

void SetTaskState( const int tid, enum TaskState to ) {
  RemoveStatefulPriorityTaskQueueNode( &scheduler, &task_descriptors[tid] );
  task_descriptors[tid].state = to;
  if ( tid != current_task->tid ) {
    EnqueueStatefulPriorityTaskQueue( &scheduler, &task_descriptors[tid] );
  }
}

enum TaskState GetTaskState( const int tid ) {
  return task_descriptors[tid].state;
}

void SetCurrentTaskState( enum TaskState to ) {
  current_task->state = to;
}

enum TaskState GetCurrentTaskState( void ) {
  return current_task->state;
}

bool IsValidTid( int tid ) {
  return 0 <= tid && tid < MAX_TASKS;
}

bool IsTaskAlive( int tid ) {
  return IsValidTid( tid ) && GetTaskState( tid ) != ZOMBIE && GetTaskState( tid ) != FREE;
}

bool IsValidPriority( int priority ) {
  return LOW_PRIORITY <= priority && priority <= HIGHEST_PRIORITY;
}

int AwaitEventHandler( int intr ) {
  Debugln( "AwaitEvent (0x%08x)", intr );

  current_task->awaiting_event_id = intr;
  SetCurrentTaskState( EVENT_BLOCKED );

  EnableInterrupt( intr );

  return RETURN_STATUS_OK;
}

void UnblockTaskWaitingOnEvent( int intr ) {
  struct priority_task_queue *event_blocked = &scheduler.states[EVENT_BLOCKED];

  for ( int prio = HIGHEST_PRIORITY; prio < NUM_PRIORITIES; prio++ ) {
    struct task *head = ( struct task* )event_blocked->priorities[prio].first;
    while ( head ) {
      if ( head->awaiting_event_id == intr ) {
        SetTaskState( head->tid, READY );
        SetTaskReturnValue( head->tid, ERR_COLLECT_DATA_AND_REENABLE_INTERRUPTS );
        return;
      }
      head = ( struct task* )head->next;
    }
  }
}

uint32_t HandleIrq( int32_t intr_code ) {
  Debugln( "INTERRUPT %d (0x%08x)", intr_code, 1 << -intr_code );

  uint32_t current_time = GetTimerTime();
  if ( TimerWentOff() ) {
    current_time = 0;
  }

  if ( intr_code < 0 ) {
    uint32_t intr = 1 << -intr_code;
    if ( IsTimerInterrupt( intr ) ) {
      if ( TimerWentOff() ) {
        ClearTimerInterrupt();
      }
    }
    else {
      DisableInterrupt( intr );
    }

    UnblockTaskWaitingOnEvent( intr );
  }

  return current_time;
}

int DestroyHandler( int tid ) {
  Debugln( "Destoying (%d)", tid );

  if ( ! IsValidTid( tid ) ) {
    Debugln( "Invalid tid, aborting!" );
    return ERR_DESTROY_IMPOSSIBLE_TID;
  }

  if ( ! IsTaskAlive( tid ) ) {
    Debugln( "Non-existent tid, aborting!" );
    return ERR_DESTROY_NONEXISTENT_TASK;
  }

  struct task *task = &task_descriptors[tid];

  /* Destroy Children */
  struct task *child;

  while (( child = ( struct task* )DequeueQueue( &task->children ) )) {
    DestroyHandler( child->tid );
  }

  /* Destroy self */
  SetTaskState( tid, FREE );
  EnqueueQueue( &available_descriptors, task );

  if ( task->awaiting_event_id >= 0 ) {
    DisableInterrupt( task->awaiting_event_id );
  }

  num_tasks -= 1;

  return RETURN_STATUS_OK;
}

void TasksStatsHandler( struct TasksStats *stats ) {
  stats->num_alive_tasks = num_tasks;

  int index = 0;

  EnqueueStatefulPriorityTaskQueue( &scheduler, current_task );

  for ( enum Priority priority = HIGHEST_PRIORITY; priority <= NULL_PRIORITY; priority++ ) {
    for ( enum TaskState state = ZOMBIE; state <= EVENT_BLOCKED; state++ ) {
      struct task *head = ( struct task* )scheduler.states[state].priorities[priority].first;
      while ( head ) {
        stats->tasks[index].tid = head->tid + MAX_TASKS * head->generation;
        stats->tasks[index].priority = head->priority;
        stats->tasks[index].state = head->state;
        stats->tasks[index].num_activates = head->num_activates;
        stats->tasks[index].allowed_user_time = head->allowed_user_time;
        stats->tasks[index].used_user_time = head->used_user_time;
        index += 1;

        head = ( struct task* )head->next;
      }
    }
  }

  RemoveStatefulPriorityTaskQueueNode( &scheduler, current_task );

  stats->num_tasks = index;
}
