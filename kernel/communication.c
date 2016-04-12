#include "communication.h"
#include "task.h"
#include "../include/common.h"
#include "../include/communication.h"
#include "../lib/string.h"

static struct msg_queue_node nodes[TASK_MAX][TASK_MAX];
static struct task_queue msg_queues[TASK_MAX];

void InitCommunication( void ) {
  for ( int i = 0; i < TASK_MAX; i++ ) {
    msg_queues[i].first = NULL;
    msg_queues[i].last = NULL;
  }
}

int SendProxy( int tid, void *msg, int msglen, void *reply, int replylen ) {
  if ( ! IsValidTid( tid ) ) {
    return ERR_SEND_IMPOSSIBLE_TID;
  }

  if ( ! IsTaskAlive( tid ) ) {
    return ERR_SEND_NONEXISTENT_TASK;
  }

  const int mytid = GetCurrentTid();

  struct msg_queue_node n;
  n.next = NULL;
  n.prev = NULL;
  n.to_tid = tid;
  n.from_tid = mytid;
  n.msg_buffer = msg;
  n.msg_buffer_size = msglen;
  n.reply_buffer = reply;
  n.reply_buffer_size = replylen;

  struct msg_queue_node *node = &nodes[tid][mytid];

  memcpy( node, &n, sizeof( n ) );

  EnqueueTaskQueue( &msg_queues[tid], node );

  SetCurrentTaskState( SEND_BLOCKED );

  if ( GetTaskState( tid ) == RECEIVE_BLOCKED ) {
    struct msg_queue_node *receiver_node = &nodes[tid][tid];

    node->put_tid = receiver_node->put_tid;
    node->receive_buffer = receiver_node->receive_buffer;
    node->receive_buffer_size = receiver_node->receive_buffer_size;

    return CompleteReceive( mytid );
  }

  return RETURN_STATUS_OK;
}

int ReceiveProxy( int *tid, void *msg, int msglen ) {
  const int mytid = GetCurrentTid();

  if ( ! msg_queues[mytid].first ) {
    SetCurrentTaskState( RECEIVE_BLOCKED );

    struct msg_queue_node n;
    n.to_tid = mytid;
    n.receive_buffer = msg;
    n.receive_buffer_size = msglen;

    memcpy( &nodes[mytid][mytid], &n, sizeof( n ) );

    return RETURN_STATUS_OK;
  }

  struct msg_queue_node *n = ( struct msg_queue_node* )( msg_queues[mytid].first );

  n->put_tid = tid;
  n->receive_buffer = msg;
  n->receive_buffer_size = msglen;

  return CompleteReceive( mytid );
}

int CompleteReceive( int tid ) {
  struct msg_queue_node *n = ( struct msg_queue_node* )DequeueTaskQueue( &msg_queues[tid] );

  memcpy( n->receive_buffer, n->msg_buffer, n->receive_buffer_size );
  *( n->put_tid ) = n->from_tid;

  SetTaskState( n->to_tid, READY );
  SetTaskState( n->from_tid, REPLY_BLOCKED );
  SetTaskReturnValue( n->to_tid, n->msg_buffer_size );

  return n->msg_buffer_size;
}

int ReplyProxy( int tid, void *reply, int replylen ) {
  const int mytid = GetCurrentTid();
  struct msg_queue_node *n = &nodes[mytid][tid];

  if ( ! IsValidTid( tid ) ) {
    return ERR_SEND_IMPOSSIBLE_TID;
  }

  if ( ! IsTaskAlive( tid ) ) {
    return ERR_SEND_NONEXISTENT_TASK;
  }

  if ( GetTaskState( tid ) != REPLY_BLOCKED ) {
    return ERR_NOT_REPLY_BLOCKED;
  }

  memcpy( n->reply_buffer, reply, n->reply_buffer_size );

  SetTaskState( n->from_tid, READY );
  SetTaskReturnValue( n->from_tid, replylen );

  if ( replylen > n->reply_buffer_size ) {
    return ERR_BUFFER_TOO_SMALL;
  }

  return RETURN_STATUS_OK;
}
