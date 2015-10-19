#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>

enum Priorities {
  NULL_PRIORITY = 100,
};

enum Tids {
  NULL_TID = 0,
};

enum {
  DEFAULT_STACK_SIZE = 0x200,
};

enum {
  TASK_MAX = 1024,
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

struct task {
  const int tid;
  enum TaskState state;
  int priority;
  const int parent_tid;
  uint32_t sp;
  uint32_t spsr;
  uint32_t pc;
  uint32_t return_value;
};

typedef struct task *Task;

void tasks_init(void);

Task Task_create(int priority, int stack_size, void (*code)(void));

void release_processor();

#endif
