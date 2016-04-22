#include "task.h"
#include "task_queue.h"
#include "../include/common.h"
#include "../include/task.h"
#include "../lib/string.h"
#include "../lib/debug.h"
#include "../lib/math.h"

#ifdef DEBUG
#define DebugInspectTask( task ) \
  DebugPrint( "#<Task:%d tid=%d priority=%d user_time=%d last_activate_at=%d num_activates=%d>", task, task->tid, task->priority, task->used_user_time / 1000, task->last_activate_at, task->num_activates );
#else
#define DebugInspectTask(...)
#endif

enum {
  NUM_REGS_POPPED = 14,
};

static int num_tasks = 0;

static struct task task_descriptors[TASK_MAX];

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
  DebugPrint( "\r\n" );
}

void ExitTask( void ) {
  Debug( "Exit from " );
  DebugInspectTask( current_task );
  DebugPrint( "\r\n" );

  current_task->state = ZOMBIE;
}

static struct task *GetNextScheduledTask( void ) {
  return DequeuePriorityTaskQueue( &scheduler.states[READY] );
}

static void ScheduleTask( struct task *task ) {
  EnqueueStatefulPriorityTaskQueue( &scheduler, task );
}

int CreateTask( int priority, void ( *code )( void ) ) {
  const int tid = AssignTid();
  if ( tid < 0 ) {
    return ERR_UNAVAILABLE_DESCRIPTOR;
  }

  uint32_t *stack = ( uint32_t* )stacks[tid];
  stack[STACK_SIZE - 1] = ( uint32_t )Exit; // task initial lr

  struct task t = {
    next: NULL,
    prev: NULL,
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
  ScheduleTask( current_task );
  current_task = GetNextScheduledTask();

  current_task->last_activate_at = GetTimerTime();
  current_task->allowed_user_time += current_task->last_activate_at / 100;
  current_task->num_activates += 1;

  Debug( "Switching to " );
  DebugInspectTask( current_task );
  DebugPrint( " (%d%% utilization)\r\n", current_task->used_user_time / max( current_task->allowed_user_time / 100, 1 ) );

  Activate( current_task->spsr, current_task->sp, current_task->pc );
}

void SaveTaskState( uint32_t spsr, uint32_t *sp, uint32_t pc, uint32_t timer ) {
	current_task->spsr = spsr;
	current_task->sp = sp;
	current_task->pc = pc;
  current_task->used_user_time += ( current_task->last_activate_at - timer ) / 100;
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
  return 0 <= tid && tid < TASK_MAX;
}

bool IsTaskAlive( int tid ) {
  return IsValidTid( tid ) && tid < num_tasks && GetTaskState( tid ) != ZOMBIE;
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
    struct task *head = ( struct task* )event_blocked->priorities[prio].last;
    while ( head ) {
      if ( head->awaiting_event_id == intr ) {
        SetTaskState( head->tid, READY );
        SetTaskReturnValue( head->tid, ERR_COLLECT_DATA_AND_REENABLE_INTERRUPTS );
        return;
      }
      head = ( struct task* )head->prev;
    }
  }
}

void HandleIrq( int32_t intr_code ) {
  Debugln( "Interrupt: %d (0x%08x)", intr_code, 1 << -intr_code );

  if ( intr_code < 0 ) {
    uint32_t intr = 1 << -intr_code;
    if ( IsTimerInterrupt( intr ) ) {
      Debugln( "Timer interrupt" );
      if ( TimerWentOff() ) {
        Debugln( "Clearing timer interrupt" );
        ClearTimerInterrupt();
      }
    }
    else {
      Debugln( "Disabling interrupt" );
      DisableInterrupt( intr );
    }

    UnblockTaskWaitingOnEvent( intr );
  }
}
