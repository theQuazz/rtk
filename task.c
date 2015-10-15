#include <string.h>
#include <stdlib.h>

#include "rtk.h"
#include "task.h"
#include "min_heap.h"

const int TASK_MAX = 1024;

MinHeap tasks[NUM_TASK_STATES] = { NULL };
Task current_task = NULL;
Task null_task = NULL;

static int sign(const int x) {
  return (x > 0) - (x < 0);
}

enum Comparison compare_tasks(const void *_a, const void *_b) {
  const Task a = (Task)_a;
  const Task b = (Task)_b;
  return sign(a->priority - b->priority);
}

static void null_process(void) {
  for ( ;; ) {}
}

int assign_tid() {
  static int tid = 0;
  return ++tid;
}

int get_current_tid() {
  return current_task ? current_task->tid : 0;
}

void *allocator(size_t size) {
  return malloc(size);
}

Task get_next_scheduled_task(void) {
  current_task = MinHeap_delete_min(tasks[READY]);
  return current_task;
}

void schedule_task(Task task) {
  MinHeap_insert(tasks[READY], task);
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
    .run = code,
  };

  memcpy(task, &t, sizeof(struct task));

  schedule_task(task);

  return task;
}

void release_processor() {
  Task task = NULL;

  if (current_task->tid != NULL_TID) {
    schedule_task(current_task);
  }

  task = get_next_scheduled_task();

  if (!task) {
    task = null_task;
  }

  task->run();
}

void tasks_init(void) {
  for (int i = 0; i < NUM_TASK_STATES; i++) {
    tasks[i] = MinHeap__allocate_and_create(TASK_MAX, compare_tasks, malloc);
  }
  current_task = null_task = Task_create(NULL_PRIORITY, null_process);
}
