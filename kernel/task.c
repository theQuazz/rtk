#include "task.h"
#include "task_queue.h"
#include "asm.h"
#include "../include/task.h"
#include "../lib/string.h"
#include "../lib/debug.h"

#ifdef DEBUG
#define DebugInspectTask( task ) \
  DebugPrint( "#<Task:%d tid=%d priority=%d spsr=%d sp=%d pc=%d>\n", task, task->tid, task->priority, task->spsr, task->sp, task->pc );
#else
#define DebugInspectTask(...)
#endif

enum {
  NUM_REGS_POPPED = 14,
};

static int num_tasks = 0;

static struct task_queue_node task_descriptors[TASK_MAX];

static struct task *current_task = NULL;

static struct stateful_priority_task_queue scheduler;

static char stacks[STACK_SIZE][TASK_MAX];

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

static int AssignTid( void ) {
  if ( num_tasks >= TASK_MAX - 1 ) return -1;
  return num_tasks++;
}

void Nop( void ) {
  Debug( "Pass " );
  DebugInspectTask( current_task );

}

void ExitTask( void ) {
  Debug( "Exit from " );
  DebugInspectTask( current_task );

  current_task->state = ZOMBIE;
}

static struct task *GetNextScheduledTask( void ) {
  return DequeuePriorityTaskQueue( &scheduler.states[READY] );
}

static void ScheduleTask( struct task *task ) {
  EnqueueStatefulPriorityTaskQueue( &scheduler, ( struct task_queue_node* )task );
}

int CreateTask( int priority, void ( *code )( void ) ) {
  const int tid = AssignTid();
  if ( tid < 0 ) {
    return ERR_UNAVAILABLE_DESCRIPTOR;
  }

  uint32_t *stack = ( uint32_t* )stacks[tid];
  stack[STACK_SIZE - 1] = ( uint32_t )Exit; // task initial lr

  struct task t = {
    tid,
    state: READY,
    priority,
    parent: current_task,
    sp: stack + STACK_SIZE - 14, // allow space for 14 registers (word size = 4 bytes) to be popped
    spsr: 0x10,
    pc: ( long )code,
  };

  struct task *task = &task_descriptors[t.tid];

  memcpy( task, &t, sizeof( struct task ) );

  ScheduleTask( task );

  Debug( "Creating " );
  DebugInspectTask( task );

  return task->tid;
}

int CreateTaskSafe( int priority, void ( *code )( void ) ) {
  if ( priority < HIGHEST_PRIORITY || LOW_PRIORITY < priority ) {
    return ERR_INVALID_PRIORITY;
  }

  return CreateTask( priority, code );
}

void ScheduleAndActivate( void ) {
  ScheduleTask( current_task );
  current_task = GetNextScheduledTask();

  Debug( "Switching to " );
  DebugInspectTask( current_task );

  Activate( current_task->spsr, current_task->sp, current_task->pc );
}

void SaveTaskState( uint32_t spsr, uint32_t *sp, uint32_t pc, uint32_t ret ) {
	current_task->spsr = spsr;
	current_task->sp = sp;
	current_task->pc = pc;
  SetCurrentTaskReturnValue( ret );
}

void SetCurrentTaskReturnValue( uint32_t ret ) {
  *( current_task->sp ) = ret; // TODO: find better way of storing return code?
}

void SetTaskReturnValue( const int tid, uint32_t ret ) {
  *( task_descriptors[tid].sp ) = ret; // TODO: find better way of storing return code?
}

void SetTaskState( const int tid, enum TaskState to ) {
  RemoveStatefulPriorityTaskQueueNode( &scheduler, &task_descriptors[tid] );
  task_descriptors[tid].state = to;
  EnqueueStatefulPriorityTaskQueue( &scheduler, &task_descriptors[tid] );
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
  return 0 <= tid && tid < TASK_MAX;
}

bool IsTaskAlive( int tid ) {
  return IsValidTid( tid ) && tid < num_tasks && GetTaskState( tid ) != ZOMBIE;
}