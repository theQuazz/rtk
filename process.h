#ifndef __PROCESS_H__
#define __PROCESS_H__

enum TaskState {
  READY,
  ACTIVE,
  ZOMBIE,
  SEND_BLOCKED,
  RECEIVE_BLOCKED,
  REPLY_BLOCKED,
  EVENT_BLOCKED
};

extern const int TASK_MAX;
extern const int NUM_TASK_STATES;

typedef struct task {
  const int tid;
  enum TaskState state;
  int priority;
  const int parent_tid;
  void *sp;
  U32 spsr;
  int return_value;
} Task;

enum CreateError {
  ERR_INVALID_PRIORITY = -1;
  ERR_UNAVAILABLE_DESCRIPTOR = -2;
};
int Create(int priority, void (*code)(void));

int MyTid(void);

enum MyParentTidError {
  ERR_PARENT_DESTROYED = -1;
}
int MyParentTid(void);

void Pass(void);

void Exit(void);

enum ProcessSendError {
  ERR_IMPOSSIBLE_TID = -1;
  ERR_NONEXISTENT_TASK = -2;
  ERR_SRR_INCOMPLETE = -3;
};
enum SendError Send(int tid, char *msg, int msglen, char *reply, int replylen);

int Receive(int *tid, char *msg, int msglen);

enum ReplyError {
  ERR_NOT_REPLY_BLOCKED = -3;
  ERR_BUFFER_TOO_SMALL = -4;
};
int Reply(int tid, char *reply, int replylen);

enum RegisterAsError {
  ERR_INVALID_NAMESERVER_TID = -1;
};
int RegisterAs(char *name);

int WhoIs(char *name);

enum DelayError {
  ERR_INVALID_CLOCKSERVER_TID = -1;
};
int Delay(int ticks);

int Time(void);

int DelayUntil(int ticks);

#endif
