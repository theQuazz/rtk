#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>

enum TaskState {
  READY,
  ACTIVE,
  ZOMBIE,
  SEND_BLOCKED,
  RECEIVE_BLOCKED,
  REPLY_BLOCKED,
  EVENT_BLOCKED,
};

enum {
  NUM_TASK_STATES = EVENT_BLOCKED + 1,
};

enum Priority {
  HIGHEST_PRIORITY = 0,
  HIGH_PRIORITY = 1,
  MEDIUM_PRIORITY = 2,
  LOW_PRIORITY = 3,
  NULL_PRIORITY = 4,
};

enum {
  NUM_PRIORITIES = NULL_PRIORITY,
};

enum Tids {
  NULL_TID = 0,
};

enum {
  STACK_SIZE = 0x2000,
};

enum {
  TASK_MAX = 1024,
};

struct task {
  const int tid;
  enum TaskState state;
  enum Priority priority;
  const int parent_tid;
  uint32_t sp;
  uint32_t spsr;
  uint32_t pc;
  uint32_t return_value;
};

#endif
