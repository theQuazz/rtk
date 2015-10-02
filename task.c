#include <string.h>

#include "task.h"
#include "min_heap.h"

const int TASK_MAX = 1000;

MinHeap tasks[NUM_TASK_STATES] = { NULL };
Task current_task = NULL;

static int sign(const int x) {
  return (x > 0) - (x < 0);
}

enum Comparison compare_tasks(const void *_a, const void *_b) {
  const Task a = (Task)_a;
  const Task b = (Task)_b;
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

void *allocator(size_t size) {
  return NULL;
}

void Task_init(Task task) {
  return;
}

Task Task_create(int priority, void (*code)(void)) {
  Task task = allocator(sizeof(struct task));

  struct task t = {
    .tid = assign_tid(),
    .state = READY,
    .priority = priority,
    .parent_tid = get_current_tid(),
    .sp = NULL, // get_new_sp();
    .spsr = 0, // get_default_spsr();
    .return_value = 0,
  };

  memcpy(task, &t, sizeof(struct task));

  Task_init(task);
  schedule_task(task);

  return task;
}

Task get_next_scheduled_task(void) {
  current_task = MinHeap_delete_min(tasks[READY]);
  return current_task;
}

void schedule_task(Task task) {
  MinHeap_insert(tasks[READY], task);
}

void release_processor() {
  Task task = NULL;

  schedule_task(current_task);
  task = get_next_scheduled_task();
}
