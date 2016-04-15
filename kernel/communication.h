#ifndef __K_COMMUNICATION_H__
#define __K_COMMUNICATION_H__

#include <stdbool.h>

#include "task_queue.h"

enum {
  MSG_QUEUE_BUFFER_SLOTS = TASK_MAX,
};

struct msg_queue_node {
  struct queue_node;
  int from_tid;
  int to_tid;
  int *put_tid;
  void *msg_buffer;
  int msg_buffer_size;
  void *reply_buffer;
  int reply_buffer_size;
  void *receive_buffer;
  int receive_buffer_size;
};

void InitCommunication( void );

int SendProxy( int tid, void *msg, int msglen, void *reply, int replylen );

int ReceiveProxy( int *tid, void *msg, int msglen );

int CompleteReceive( int tid );

int ReplyProxy( int tid, void *reply, int replylen );

void CompleteReply( int tid );


#endif
