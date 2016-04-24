#ifndef __K_TASK_H__
#define __K_TASK_H__

#include <stdint.h>
#include <stdbool.h>

#include "system.h"
#include "../include/task_priority.h"
#include "../include/interrupt.h"
#include "../include/task.h"
#include "../lib/queue.h"

enum TaskState {
  FREE = -1,
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
  STACK_SIZE_BYTES = 0x2000,
  STACK_SIZE_WORDS = STACK_SIZE_BYTES / sizeof( uint32_t ),
};

struct task {
  struct queue_node;
  int tid;
  int generation;
  enum TaskState state;
  enum Priority priority;
  struct task *parent;
  uint32_t *stack;
  uint32_t *sp;
  uint32_t spsr;
  uint32_t pc;
  int awaiting_event_id;
  long num_activates;
  uint32_t last_activate_at;
  unsigned long allowed_user_time;
  unsigned long used_user_time;
  struct queue_node siblings;
  struct queue children;
};

void InitTasks( void );

int GetCurrentTid( void );

int GetParentTid( void );

enum TaskState GetCurrentTaskState( void );
void SetCurrentTaskState( enum TaskState to );

enum TaskState GetTaskState( const int tid );
void SetTaskState( const int tid, enum TaskState to );

int GetCurrentTaskPriority( void );
int SetCurrentTaskPriority( enum Priority to );

int GetTaskPriority( const int tid );
int SetTaskPriority( const int tid, enum Priority to );

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

bool IsValidPriority( int priority );

int AwaitEventHandler( int intr );

void UnblockTaskWaitingOnEvent( int intr );

uint32_t HandleIrq( int32_t intr_code );

void HandleDestroy( const int tid );

void TasksStatsHandler( struct TasksStats *stats );

#endif
