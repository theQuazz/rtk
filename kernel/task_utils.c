#include "task.h"
#include "task_utils.h"
#include "task_queue.h"
#include "asm.h"
#include "../lib/string.h"
#include "../lib/print.h"
#include "../arch/arm/mmio.h"

struct task_queue_node task_descriptors[TASK_MAX];

struct task *current_task = NULL;
struct task *null_task = NULL;

struct stateful_priority_task_queue scheduler;

const char stacks[STACK_SIZE][TASK_MAX];

static void null_process( void ) {
  for ( ;; ) {
    Print( 0, "Null process...\n" );
    syscall();
  }
}

int assign_tid() {
  static int tid = 0;
  return tid++;
}

int get_current_tid() {
  return current_task ? current_task->tid : 0;
}

struct task *get_next_scheduled_task( void ) {
  return dequeue_priority_task_queue( &scheduler.states[READY] );
}

void schedule_task( struct task *task ) {
  enqueue_stateful_priority_task_queue( &scheduler, ( struct task_queue_node* )task );
}

void Task_inspect( struct task *task ) {
  Print( 0, "#<Task:%d tid=%d priority=%d spsr=%d sp=%d pc=%d>\n", task, task->tid, task->priority, task->spsr, task->sp, task->pc );
}

struct task *Task_create( int priority, void ( *code )( void ) ) {
  const int tid = assign_tid();
  const void *stack = stacks[tid];

  struct task t = {
    tid: tid,
    state: READY,
    priority: priority,
    parent_tid: get_current_tid(),
    sp: ( long )stack + STACK_SIZE - 14 * 4, // allow space for 14 registers (word size = 4 bytes) to be popped
    spsr: 0x10,
    pc: ( long )code,
    return_value: 0,
  };

  struct task *task = &task_descriptors[t.tid];

  memcpy( task, &t, sizeof( struct task ) );

  schedule_task( task );

  Print( 0, "Task_create( %d, %d ) from %d\n", priority, code, current_task->tid );
  Task_inspect( task );

  return task;
}

void save_task_state( uint32_t spsr, uint32_t sp, uint32_t pc ) {
	current_task->spsr = spsr;
	current_task->sp = sp;
	current_task->pc = pc;
}

void release_processor() {
  Print( 0, "Release processor by %d...\n", current_task->tid );
  for ( int j = 0; j < 600000000; j++ ) { j = j + 1; }

  if ( current_task && current_task != null_task ) {
    schedule_task( current_task );
  }

  current_task = get_next_scheduled_task();

  if ( ! current_task ) {
    current_task = null_task;
  }

  Print( 0, "Switching to " );
  Task_inspect( current_task );
  activate( current_task->spsr, current_task->sp, current_task->pc );
}

void tasks_init( void ) {
  null_task = Task_create( NULL_PRIORITY, null_process );
}
