#include "communication.h"
#include "task.h"
#include "../include/common.h"
#include "../include/communication.h"
#include "../lib/string.h"
#include "../lib/debug.h"
#include "../lib/math.h"

static struct msg_queue_node nodes[MAX_TASKS];
static struct queue msg_queues[MAX_TASKS];

void InitCommunication( void ) {
  for ( int i = 0; i < MAX_TASKS; i++ ) {
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

  struct msg_queue_node *node = &nodes[mytid % MAX_TASKS];

  memcpy( node, &n, sizeof( n ) );

  EnqueueQueue( &msg_queues[tid % MAX_TASKS], node );

  SetCurrentTaskState( RECEIVE_BLOCKED );

  if ( GetTaskState( tid ) == SEND_BLOCKED ) {
    Debugln( "Task %d awaiting message, resuming", tid );

    struct msg_queue_node *receiver_node = &nodes[tid % MAX_TASKS];

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

  if ( ! msg_queues[mytid % MAX_TASKS].first ) {
    Debugln( "No messages in queue, waiting" );

    SetCurrentTaskState( SEND_BLOCKED );

    struct msg_queue_node n;
    n.put_tid = tid;
    n.receive_buffer = msg;
    n.receive_buffer_size = msglen;

    memcpy( &nodes[mytid % MAX_TASKS], &n, sizeof( n ) );

    return RETURN_STATUS_OK;
  }

  struct msg_queue_node *n = ( struct msg_queue_node* )( msg_queues[mytid % MAX_TASKS].last );

  Debugln( "Message available, retrieving (%d)", n->msg_buffer );

  n->put_tid = tid;
  n->receive_buffer = msg;
  n->receive_buffer_size = msglen;

  return CompleteReceive( mytid );
}

int CompleteReceive( int tid ) {
  struct msg_queue_node *n = ( struct msg_queue_node* )DequeueQueue( &msg_queues[tid % MAX_TASKS] );

  *( n->put_tid ) = n->from_tid;
  if ( n->receive_buffer != NULL && n->msg_buffer != NULL ) {
    memcpy( n->receive_buffer, n->msg_buffer, min( n->receive_buffer_size, n->msg_buffer_size ) );
  }

  SetTaskState( n->from_tid, REPLY_BLOCKED );
  SetTaskState( n->to_tid, READY );
  SetTaskReturnValue( n->to_tid, n->msg_buffer_size );

  Debugln( "Message (%d) sent from task %d to task %d", n->msg_buffer, n->from_tid, n->to_tid );

  return n->msg_buffer_size;
}

int ReplyProxy( int tid, void *reply, int replylen ) {
  Debugln( "Replying to task %d", tid );

  struct msg_queue_node *n = &nodes[tid % MAX_TASKS];

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

  if ( n->reply_buffer != NULL && reply !=  NULL ) {
    memcpy( n->reply_buffer, reply, min( n->reply_buffer_size, replylen ) );
  }

  SetTaskState( n->from_tid, READY );
  SetTaskReturnValue( n->from_tid, replylen );

  if ( replylen > n->reply_buffer_size ) {
    Debugln( "Reply buffer is too small! (%d vs %d)", replylen, n->reply_buffer_size );
    return ERR_BUFFER_TOO_SMALL;
  }

  return RETURN_STATUS_OK;
}
