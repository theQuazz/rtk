#include "semaphore.h"
#include "../include/communication.h"
#include "../include/nameserver.h"
#include "../include/task.h"

static char SEM_SERVER_NAME[] = "semahpore-server";

struct sem_queue_node {
  struct queue_node;
  int tid;
};

enum SemReqType {
  SEM_V,
  SEM_P,
};

struct SemReq {
  struct BaseMsg;
  struct Semaphore *sem;
};

void SemV( struct Semaphore *s ) {
  struct SemReq req = {
    .type = SEM_V,
    .sem = s,
  };

  Send( WhoIs( SEM_SERVER_NAME ), &req, sizeof( req ), NULL, 0 );
}

void SemP( struct Semaphore *s ) {
  struct SemReq req = {
    .type = SEM_P,
    .sem = s,
  };

  Send( WhoIs( SEM_SERVER_NAME ), &req, sizeof( req ), NULL, 0 );
}

void SemInit( struct Semaphore *s, int initial ) {
  s->counter = initial;
  s->blocked.first = NULL;
  s->blocked.last = NULL;
}

void SemaphoreServer( void ) {
  while ( RegisterAs( SEM_SERVER_NAME ) == ERR_INVALID_NAMESERVER_TID );

  struct sem_queue_node nodes[MAX_TASKS];

  for ( ;; ) {
    struct SemReq req;
    int sender_tid;
    Receive( &sender_tid, &req, sizeof( req ) );

    switch ( req.type ) {
      case SEM_V: {
        req.sem->counter -= 1;
        if ( req.sem->counter < 0 ) {
          nodes[sender_tid].tid = sender_tid;
          EnqueueQueue( &req.sem->blocked, &nodes[sender_tid] );
        }
        break;
      }

      case SEM_P: {
        req.sem->counter += 1;
        if ( req.sem->counter <= 0 ) {
          struct sem_queue_node *n = ( struct sem_queue_node* )DequeueQueue( &req.sem->blocked );
          Reply( n->tid, NULL, 0 );
        }
        break;
      }

      default: break;
    }
  }
}
