#include "rtk.h"
#include "task.h"
#include "min_heap.h"
#include "string.h"
#include "asm.h"
#include "memory.h"
#include "mmio.h"
#include "io.h"

struct min_heap tasks[NUM_TASK_STATES];
void *min_heap_tree_space[NUM_TASK_STATES][TASK_MAX];

Task current_task = NULL;
Task null_task = NULL;

static int sign(const int x) {
  return (x > 0) - (x < 0);
}

static void Task_inspect(void *_t) {
  Task t = _t;
  printf("#<Task:%d @tid=%d @priority=%d @scheduled_at=%d>", t, t->tid, t->priority, t->scheduled_at);
}

enum Comparison compare_tasks(const void *_a, const void *_b) {
  const Task a = (Task)_a;
  const Task b = (Task)_b;
  return a->priority == b->priority ?
    sign(a->scheduled_at - b->scheduled_at) :
    sign(a->priority - b->priority);
}

static void null_process(void) {
  for ( ;; ) {
    uart_puts("null...\n");
    for (int i = 0; i < 100000000; i++) {}
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

int get_current_time() {
  static volatile int fake_time = 0;
  return fake_time++;
}

Task get_next_scheduled_task(void) {
  return MinHeap_delete_min(&tasks[READY]);
}

void schedule_task(Task task) {
  task->scheduled_at = get_current_time();
  MinHeap_insert(&tasks[READY], task);
}

Task Task_create(int priority, void (*code)(void)) {
  void *stack = alloc_stack();

  struct task t = {
    .tid = assign_tid(),
    .state = READY,
    .priority = priority,
    .parent_tid = get_current_tid(),
    .scheduled_at = get_current_time(),
    .sp = (long)stack + STACK_SIZE - 14 * 4, // allow space for 14 registers (word size = 4 bytes) to be popped
    .spsr = 0x10,
    .pc = (long)code,
    .return_value = 0,
  };

  Task task = stack;

  memcpy(task, &t, sizeof(struct task));

  schedule_task(task);

  return task;
}

void save_task_state(uint32_t spsr, uint32_t sp, uint32_t pc) {
	current_task->spsr = spsr;
	current_task->sp = sp;
	current_task->pc = pc;
}

void release_processor() {

  if (current_task && current_task != null_task) {
    schedule_task(current_task);
  }

  current_task = get_next_scheduled_task();

  if (!current_task) {
    current_task = null_task;
  }

  activate(current_task->spsr, current_task->sp, current_task->pc);
}

void tasks_init(void) {
  for (int i = 0; i < NUM_TASK_STATES; i++) {
    MinHeap_init(&tasks[i], &min_heap_tree_space[i], TASK_MAX, compare_tasks);
  }

  null_task = Task_create(NULL_PRIORITY, null_process);
}
