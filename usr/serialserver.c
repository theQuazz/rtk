#include "serialserver.h"
#include "../include/task.h"
#include "../include/nameserver.h"
#include "../include/io.h"
#include "../include/interrupt.h"
#include "../include/communication.h"
#include "../lib/queue.h"

#include <stddef.h>

char GetReceivedChar( void );

enum SerialRequestType {
  SS_RECEIVED,
  SS_TRANSMITTED,
  SS_TRANSMIT,
  SS_RECEIVE,
};

struct SerialRequest {
  struct BaseMsg;
  char buffer[SS_BUFFER_SIZE];
};

struct receive_node {
  struct queue_node;
  int tid;
};

struct transmit_node {
  struct queue_node;
  char ch;
};

int Putc( int channel, char ch ) {
  const int serial_tid = WhoIs( "io" );

  if ( serial_tid < 0 ) {
    return ERR_INVALID_SERIAL_SERVER_TID;
  }

  struct SerialRequest req = {
    .type = SS_TRANSMIT,
    .buffer = { ch },
  };
  Send( serial_tid, &req, sizeof( req ), NULL, 0 );

  return RETURN_STATUS_OK;
}

int Getc( int channel ) {
  const int serial_tid = WhoIs( "io" );

  if ( serial_tid < 0 ) {
    return ERR_INVALID_SERIAL_SERVER_TID;
  }

  char ch;

  struct SerialRequest req = {
    .type = SS_RECEIVE,
  };
  Send( serial_tid, &req, sizeof( req ), &ch, sizeof( ch ) );

  return ch;
}

void ReceivedNotifier( void ) {
  const int serial_tid = MyParentTid();

  struct SerialRequest req = {
    .type = SS_RECEIVED,
  };

  for ( ;; ) {
    AwaitEvent( CHAR_RECEIVED );
    req.buffer[0] = GetReceivedChar();
    Send( serial_tid, &req, sizeof( req ), NULL, 0 );
  }
}

void TransmittedNotifier( void ) {
  const int serial_tid = MyParentTid();

  struct SerialRequest req = {
    .type = SS_TRANSMITTED,
  };

  for ( ;; ) {
    AwaitEvent( CHAR_TRANSMIT );
    Send( serial_tid, &req, sizeof( req ), NULL, 0 );
  }
}

void SerialServer( void ) {
  while ( RegisterAs( "io" ) == ERR_INVALID_NAMESERVER_TID );

  Create( HIGHEST_PRIORITY, ReceivedNotifier );
  Create( HIGHEST_PRIORITY, TransmittedNotifier );

  struct transmit_node transmit_nodes[1024];
  struct receive_node receive_nodes[1024];
  struct queue transmit_queue;
  struct queue receive_queue;

  for ( ;; ) {
    struct SerialRequest req;
    int from_tid;
    Receive( &from_tid, &req, sizeof( req ) );

    switch ( req.type ) {
      case SS_RECEIVED: {
        Reply( from_tid, NULL, 0 );
        break;
      }
      case SS_TRANSMITTED: {
        Reply( from_tid, NULL, 0 );
        break;
      }
      case SS_RECEIVE: {
        break;
      }
      case SS_TRANSMIT: {
        transmit_nodes[from_tid].ch = req.buffer[0];
        EnqueueQueue( &transmit_queue, &transmit_nodes[from_tid] );
        Reply( from_tid, NULL, 0 );
        break;
      }
      default: break;
    }
  }
}
