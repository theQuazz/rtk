#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>

enum Priorities {
  NULL_PRIORITY = 100,
};

enum Tids {
  NULL_TID = 0,
};

enum TaskState {
  READY,
  ACTIVE,
  ZOMBIE,
  SEND_BLOCKED,
  RECEIVE_BLOCKED,
  REPLY_BLOCKED,
  EVENT_BLOCKED,
  NUM_TASK_STATES,
};

extern const int TASK_MAX;

struct task {
  const int tid;
  enum TaskState state;
  int priority;
  const int parent_tid;
  void *sp;
  uint32_t spsr;
  int return_value;
  void (*run)(void);
};

typedef struct task *Task;

void tasks_init(void);

Task Task_create(int priority, void (*code)(void));

void release_processor();

#endif
