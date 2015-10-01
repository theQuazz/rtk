#include "task.h"
#include "minheap.h"

const int TASK_MAX = 1000;
const int NUM_TASK_STATES = EVENT_BLOCKED + 1;

MinHeap tasks[NUM_TASK_STATES] = { NULL };
Task current_task = NULL;

int sign(const int x) {
  return (x > 0) - (x < 0);
}

enum Comparison compare_tasks(const Task a, const Task b) {
  return sign(a->priority - b->priority);
}

void init_tasks(void) {
  for (int i = 0; i < NUM_TASK_STATES; i++) {
    tasks[i] = MinHeap_create(TASK_MAX, compare_tasks, malloc);
  }
}

int assign_tid() {
  static int tid = 0;
  return ++tid;
}

int get_current_tid() {
  return current_task ? current_task->tid : 0;
}

Task Task_create(int priority, void (*code)(void)) {
  Task task = tasks->allocator(sizeof struct task);

  task->tid = assign_tid();
  task->state = READY;
  task->priority = priority;
  task->parent_tid = get_current_tid();
  task->sp = NULL; // get_new_sp();
  task->spsr = NULL; // get_default_spsr();
  task->return_value = 0;

  Task_init(task);

  schedule_task(task);

  return task;
}

void Task_init(Task task) {
  return;
}

Task *get_next_scheduled_task(void) {
  current_task = MinHeap_delete_min(tasks);
  return current_task;
}

void *schedule_task(Task task) {
  MinHeap_insert(tasks, task);
}

void release_processor() {
  Task task = NULL;

  schedule_task(current_task);
  task = get_next_scheduled_task();

  task->sp();
}
