#ifndef __K_TASK_H__
#define __K_TASK_H__

#include <stdint.h>

#include "../include/task_priority.h"

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

enum {
  NUM_PRIORITIES = NULL_PRIORITY + 1,
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
  struct task *parent;
  uint32_t *sp;
  uint32_t spsr;
  uint32_t pc;
};

int GetCurrentTid( void );

int GetParentTid( void );

void SaveTaskState( uint32_t spsr, uint32_t *sp, uint32_t pc, uint32_t ret );

int CreateTask( int priority, void ( *code )( void ) );

int CreateTaskSafe( int priority, void ( *code )( void ) );

void ScheduleAndActivate( void );

void Nop( void );

void ExitTask( void );

#endif
