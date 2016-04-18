#ifndef __K_TASK_H__
#define __K_TASK_H__

#include <stdint.h>
#include <stdbool.h>

#include "system.h"
#include "../include/task_priority.h"
#include "../include/interrupt.h"
#include "../lib/queue.h"

enum TaskState {
  ZOMBIE,
  READY,
  ACTIVE,
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
  struct queue_node;
  const int tid;
  enum TaskState state;
  enum Priority priority;
  struct task *parent;
  uint32_t *sp;
  uint32_t spsr;
  uint32_t pc;
  enum SystemEvent awaiting_event_id;
  long num_activates;
  uint32_t last_activate_at;
  long allowed_user_time;
  long used_user_time;
};

int GetCurrentTid( void );

int GetParentTid( void );

enum TaskState GetCurrentTaskState( void );
void SetCurrentTaskState( enum TaskState to );

enum TaskState GetTaskState( const int tid );
void SetTaskState( const int tid, enum TaskState to );

void SetTaskReturnValue( const int tid, uint32_t ret );

void SetCurrentTaskReturnValue( uint32_t ret );

void SaveTaskState( uint32_t spsr, uint32_t *sp, uint32_t pc, uint32_t timer );

int CreateTask( int priority, void ( *code )( void ) );

int CreateTaskSafe( int priority, void ( *code )( void ) );

void ScheduleAndActivate( void );

void Nop( void );

void ExitTask( void );

bool IsValidTid( int tid );

bool IsTaskAlive( int tid );

int AwaitEventHandler( int eventid );

void UnblockTaskWaitingOnEvent( enum SystemEvent event );

void HandleIrq( int32_t intr_code );

#endif
