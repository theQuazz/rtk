#include "serialserver.h"
#include "../include/task.h"
#include "../include/nameserver.h"
#include "../include/io.h"
#include "../include/interrupt.h"
#include "../include/communication.h"
#include "../include/clockserver.h"
#include "../lib/cyclicbuffer.h"
#include "../lib/debug.h"
#include "../kernel/system.h"

#include <stddef.h>

#define DebugCyclicBufferInspect( c ) \
  Debugln( "#<CyclicBuffer:%d size=%d data_size=%d data=%d start=%d num_els=%d>", (c), (c)->size, (c)->data_size, (c)->data, (c)->start, (c)->num_els );

enum SerialRequestType {
  SS_RECEIVED,
  SS_TRANSMITTED,
  SS_TRANSMIT,
  SS_RECEIVE,
};

struct SerialRequest {
  struct BaseMsg;
  char data;
};

int Putc( int channel, char ch ) {
  if ( channel == 0 ) {
    bwputc( ch );
    return 0;
  }

  const int serial_tid = WhoIs( "io-transmitter" );

  if ( serial_tid < 0 ) {
    return ERR_INVALID_SERIAL_SERVER_TID;
  }

  struct SerialRequest req = {
    .type = SS_TRANSMIT,
    .data = ch,
  };
  Send( serial_tid, &req, sizeof( req ), NULL, 0 );

  return RETURN_STATUS_OK;
}

int Getc( int channel ) {
  if ( channel == 0 ) {
    return bwgetc();
  }

  const int serial_tid = WhoIs( "io-receiver" );

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
  while ( RegisterAs( "io-received-notifier" ) == ERR_INVALID_NAMESERVER_TID );

  const int serial_tid = MyParentTid();

  struct SerialRequest req = {
    .type = SS_RECEIVED,
  };

  for ( ;; ) {
    AwaitEvent( CHAR_RECEIVED );
    Debugln( "CHAR_RECEIVED" );
    req.data = GetReceivedChar( 0 );
    Send( serial_tid, &req, sizeof( req ), NULL, 0 );
  }
}

void TransmittedNotifier( void ) {
  while ( RegisterAs( "io-transmitted-notifier" ) == ERR_INVALID_NAMESERVER_TID );

  const int serial_tid = MyParentTid();

  struct SerialRequest req = {
    .type = SS_TRANSMITTED,
  };

  for ( ;; ) {
    AwaitEvent( CHAR_TRANSMIT );
    Debugln( "CHAR_TRANSMIT" );
    Send( serial_tid, &req, sizeof( req ), NULL, 0 );
  }
}

void SerialServerReceiver( void ) {
  while ( RegisterAs( "io-receiver" ) == ERR_INVALID_NAMESERVER_TID );

  int serial_tid  = -1;

  do { serial_tid = WhoIs( "io" ); }
  while ( serial_tid < 0 );

  for ( ;; ) {
    int sender_tid;
    struct SerialRequest req;
    char res;
    Receive( &sender_tid, &req, sizeof( req ) );
    Send( serial_tid, &req, sizeof( req ), &res, sizeof( res ) );
    Reply( sender_tid, &res, sizeof( res ) );
  }
}

void SerialServerTransmitter( void ) {
  while ( RegisterAs( "io-transmitter" ) == ERR_INVALID_NAMESERVER_TID );

  int serial_tid  = -1;

  do { serial_tid = WhoIs( "io" ); }
  while ( serial_tid < 0 );

  for ( ;; ) {
    int sender_tid;
    struct SerialRequest req;
    Receive( &sender_tid, &req, sizeof( req ) );
    Send( serial_tid, &req, sizeof( req ), NULL, 0 );
    Reply( sender_tid, NULL, 0 );
  }
}

void SerialServer( void ) {
  Debugln( "SerialServer starting" );

  while ( RegisterAs( "io" ) == ERR_INVALID_NAMESERVER_TID );

  Debugln( "SerialServer registered" );

  int receiver_tid;
  int transmitter_tid;
  int received_notifier_tid;
  int transmitted_notifier_tid;

  Create( HIGHEST_PRIORITY, ReceivedNotifier );
  Create( HIGHEST_PRIORITY, TransmittedNotifier );
  Create( HIGH_PRIORITY, SerialServerReceiver );
  Create( HIGH_PRIORITY, SerialServerTransmitter );

  do { received_notifier_tid = WhoIs( "io-received-notifier" ); } while ( received_notifier_tid < 0 );
  do { transmitted_notifier_tid = WhoIs( "io-transmitted-notifier" ); } while ( transmitted_notifier_tid < 0 );
  do { receiver_tid = WhoIs( "io-receiver" ); } while ( receiver_tid < 0 );
  do { transmitter_tid = WhoIs( "io-transmitter" ); } while ( transmitter_tid < 0 );

  Debugln( "SerialServer ready" );

  char tbuffer[128];
  struct cyclic_buffer transmit_buffer = {
    .size = sizeof( tbuffer ),
    .data_size = sizeof( tbuffer[0] ),
    .data = tbuffer,
    .start = 0,
    .num_els = 0,
  };
  DebugCyclicBufferInspect( &transmit_buffer );

  char rbuffer[128];
  struct cyclic_buffer receive_buffer = {
    .size = sizeof( rbuffer ),
    .data_size = sizeof( rbuffer[0] ),
    .data = rbuffer,
    .start = 0,
    .num_els = 0,
  };

  bool waiting_on_receive = false;
  bool waiting_on_transmit = true;

  for ( ;; ) {
    struct SerialRequest req;
    int from_tid;
    Receive( &from_tid, &req, sizeof( req ) );

    switch ( req.type ) {
      case SS_RECEIVED: {
        CyclicBufferPush( &receive_buffer, &req.data );
        if ( ! IsCyclicBufferFull( &receive_buffer ) ) {
          Reply( received_notifier_tid, NULL, 0 );
        }

        if ( waiting_on_receive ) {
          waiting_on_receive = false;
          char tmp;
          CyclicBufferPop( &receive_buffer, &tmp );
          Reply( receiver_tid, &tmp, sizeof( tmp ) );
        }

        break;
      }

      case SS_RECEIVE: {
        if ( IsCyclicBufferEmpty( &receive_buffer ) ) {
          waiting_on_receive = true;
        }
        else {
          bool was_full = IsCyclicBufferFull( &receive_buffer );

          char tmp;
          CyclicBufferPop( &receive_buffer, &tmp );
          Reply( receiver_tid, &tmp, sizeof( tmp ) );

          if ( was_full ) {
            Reply( received_notifier_tid, NULL, 0 );
          }
        }

        break;
      }

      case SS_TRANSMITTED: {
        if ( IsCyclicBufferEmpty( &transmit_buffer ) ) {
          waiting_on_transmit = true;
        }
        else {
          bool was_full = IsCyclicBufferFull( &transmit_buffer );

          char tmp;
          CyclicBufferPop( &transmit_buffer, &tmp );
          Reply( transmitted_notifier_tid, NULL, 0 );
          TransmitChar( 0, tmp );

          if ( was_full ) {
            Reply( transmitter_tid, NULL, 0 );
          }
        }

        break;
      }

      case SS_TRANSMIT: {
        CyclicBufferPush( &transmit_buffer, &req.data );
        if ( ! IsCyclicBufferFull( &transmit_buffer ) ) {
          Reply( transmitter_tid, NULL, 0 );
        }

        if ( waiting_on_transmit ) {
          waiting_on_transmit = false;
          Reply( transmitted_notifier_tid, NULL, 0 );
          char tmp;
          CyclicBufferPop( &transmit_buffer, &tmp );
          TransmitChar( 0, tmp );
        }

        break;
      }

      default: break;
    }
  }
}
