#include "communication.h"
#include "task.h"
#include "../include/common.h"
#include "../include/communication.h"
#include "../lib/string.h"
#include "../lib/debug.h"

static struct msg_queue_node nodes[TASK_MAX];
static struct task_queue msg_queues[TASK_MAX];

void InitCommunication( void ) {
  for ( int i = 0; i < TASK_MAX; i++ ) {
    msg_queues[i].first = NULL;
    msg_queues[i].last = NULL;
  }
}

int SendProxy( int tid, void *msg, int msglen, void *reply, int replylen ) {
  Debugln( "Sending message (%d) to %d", msg, tid );

  if ( ! IsValidTid( tid ) ) {
    Debugln( "Invalid tid, aborting!" );
    return ERR_SEND_IMPOSSIBLE_TID;
  }

  if ( ! IsTaskAlive( tid ) ) {
    Debugln( "Non-existent tid, aborting!" );
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

  struct msg_queue_node *node = &nodes[mytid];

  memcpy( node, &n, sizeof( n ) );

  EnqueueTaskQueue( &msg_queues[tid], node );

  SetCurrentTaskState( SEND_BLOCKED );

  if ( GetTaskState( tid ) == RECEIVE_BLOCKED ) {
    Debugln( "Task %d awaiting message, resuming", tid );

    struct msg_queue_node *receiver_node = &nodes[tid];

    node->put_tid = receiver_node->put_tid;
    node->receive_buffer = receiver_node->receive_buffer;
    node->receive_buffer_size = receiver_node->receive_buffer_size;

    return CompleteReceive( tid );
  }

  return RETURN_STATUS_OK;
}

int ReceiveProxy( int *tid, void *msg, int msglen ) {
  Debugln( "Receiving message" );

  const int mytid = GetCurrentTid();

  if ( ! msg_queues[mytid].first ) {
    Debugln( "No messages in queue, waiting" );

    SetCurrentTaskState( RECEIVE_BLOCKED );

    struct msg_queue_node n;
    n.put_tid = tid;
    n.receive_buffer = msg;
    n.receive_buffer_size = msglen;

    memcpy( &nodes[mytid], &n, sizeof( n ) );

    return RETURN_STATUS_OK;
  }

  struct msg_queue_node *n = ( struct msg_queue_node* )( msg_queues[mytid].first );

  Debugln( "Message available, retrieving (%d)", n->msg_buffer );

  n->put_tid = tid;
  n->receive_buffer = msg;
  n->receive_buffer_size = msglen;

  return CompleteReceive( mytid );
}

int CompleteReceive( int tid ) {
  struct msg_queue_node *n = ( struct msg_queue_node* )DequeueTaskQueue( &msg_queues[tid] );

  memcpy( n->receive_buffer, n->msg_buffer, n->receive_buffer_size );
  *( n->put_tid ) = n->from_tid;

  SetTaskState( n->from_tid, REPLY_BLOCKED );
  SetTaskState( n->to_tid, READY );
  SetTaskReturnValue( n->to_tid, n->msg_buffer_size );

  Debugln( "Message (%d) sent from task %d to task %d", n->msg_buffer, n->from_tid, n->to_tid );

  return n->msg_buffer_size;
}

int ReplyProxy( int tid, void *reply, int replylen ) {
  Debugln( "Replying to task %d", tid );

  struct msg_queue_node *n = &nodes[tid];

  if ( ! IsValidTid( tid ) ) {
    Debugln( "Invalid tid, aborting!" );
    return ERR_REPLY_IMPOSSIBLE_TID;
  }

  if ( ! IsTaskAlive( tid ) ) {
    Debugln( "Non-existent tid, aborting!" );
    return ERR_REPLY_NONEXISTENT_TASK;
  }

  if ( GetTaskState( tid ) != REPLY_BLOCKED ) {
    Debugln( "Task is not reply blocked, aborting!" );
    return ERR_NOT_REPLY_BLOCKED;
  }

  memcpy( n->reply_buffer, reply, n->reply_buffer_size );

  SetTaskState( n->from_tid, READY );
  SetTaskReturnValue( n->from_tid, replylen );

  if ( replylen > n->reply_buffer_size ) {
    Debugln( "Reply buffer is too small!" );
    return ERR_BUFFER_TOO_SMALL;
  }

  return RETURN_STATUS_OK;
}
