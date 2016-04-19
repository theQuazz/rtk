#include "serialserver.h"
#include "../include/task.h"
#include "../include/nameserver.h"
#include "../include/io.h"
#include "../include/interrupt.h"
#include "../include/communication.h"
#include "../include/clockserver.h"
#include "../lib/cyclicbuffer.h"
#include "../lib/debug.h"
#include "../arch/arm/versatilepb.h"

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

  Debugln( "wtf %c", ch );
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
  const int serial_tid = MyParentTid();

  struct SerialRequest req = {
    .type = SS_RECEIVED,
  };

  *( UART0 + UARTIMSC ) |= UARTIMSC_RXIM;

  for ( ;; ) {
    AwaitEvent( PIC_UART0 );
    if ( *( UART0 + UARTFR ) & UARTFR_RXFF ) {
      *( UART0 + UARTICR ) = UARTICR_RXIC;
      req.data = *UART0;
      Debugln( "CHAR_RECEIVED (%c)", req.data );
      Send( serial_tid, &req, sizeof( req ), NULL, 0 );
    }
    else {
      Pass();
    }
  }
}

void TransmittedNotifier( void ) {
  int courier;

  *( UART0 + UARTIMSC ) |= UARTIMSC_TXIM;

  Receive( &courier, NULL, 0 );
  Reply( courier, NULL, 0 );

  for ( ;; ) {
    char ch;
    Receive( &courier, &ch, sizeof ( ch ) );
    Debugln( "outputting %c", ch );
    *UART0 = ch;

    for ( ;; ) {
      AwaitEvent( PIC_UART0 );

      if ( *( UART0 + UARTFR ) & UARTFR_TXFE ) {
        *( UART0 + UARTICR ) = UARTICR_TXIC;
        Debugln( "CHAR_TRANSMIT" );
        Reply( courier, NULL, 0 );
        break;
      }
    }
  }
}

void TransmittedCourier( void ) {
  int notifier;
  int server;

  Receive( &server, &notifier, sizeof( notifier) );
  Send( notifier, NULL, 0, NULL, 0 );
  Reply( server, NULL, 0 );

  struct SerialRequest req = {
    .type = SS_TRANSMITTED,
  };

  char res; 

  for ( ;; ) {
    Send( server, &req, sizeof( req ), &res, sizeof( res ) );
    Send( notifier, &res, sizeof( res ), NULL, 0 );
  }
}

void SerialServerReceiver( void ) {
  int serial_tid = MyParentTid();

  while ( RegisterAs( "io-receiver" ) == ERR_INVALID_NAMESERVER_TID );

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
  int serial_tid = MyParentTid();

  while ( RegisterAs( "io-transmitter" ) == ERR_INVALID_NAMESERVER_TID );

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

  int received_notifier_tid = Create( HIGHEST_PRIORITY, ReceivedNotifier );
  int receiver_tid = Create( HIGH_PRIORITY, SerialServerReceiver );

  int transmitted_notifier_tid = Create( HIGHEST_PRIORITY, TransmittedNotifier );
  int transmitted_courier_tid = Create( HIGH_PRIORITY, TransmittedCourier );
  int transmitter_tid = Create( HIGH_PRIORITY, SerialServerTransmitter );

  Send( transmitted_courier_tid, &transmitted_notifier_tid, sizeof( transmitted_notifier_tid ), NULL, 0 );

  char tbuffer[128];
  struct cyclic_buffer transmit_buffer = {
    .size = sizeof( tbuffer ),
    .data_size = sizeof( tbuffer[0] ),
    .data = tbuffer,
    .start = 0,
    .num_els = 0,
  };

  char rbuffer[128];
  struct cyclic_buffer receive_buffer = {
    .size = sizeof( rbuffer ),
    .data_size = sizeof( rbuffer[0] ),
    .data = rbuffer,
    .start = 0,
    .num_els = 0,
  };

  int waiting_on_receive = 0;
  bool waiting_on_transmit = true;

  Debugln( "SerialServer ready" );

  for ( ;; ) {
    struct SerialRequest req;
    int from_tid;
    Receive( &from_tid, &req, sizeof( req ) );

    switch ( req.type ) {
      case SS_RECEIVED: {
        if ( ! IsCyclicBufferFull( &receive_buffer ) ) {
          Reply( received_notifier_tid, NULL, 0 );
        }

        if ( waiting_on_receive > 0 ) {
          waiting_on_receive--;
          Reply( receiver_tid, &req.data, sizeof( req.data ) );
        }
        else {
          CyclicBufferPush( &receive_buffer, &req.data );
        }

        break;
      }

      case SS_RECEIVE: {
        if ( IsCyclicBufferEmpty( &receive_buffer ) ) {
          waiting_on_receive += 1;
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
          Reply( transmitted_courier_tid, &tmp, tmp );

          if ( was_full ) {
            Reply( transmitter_tid, NULL, 0 );
          }
        }

        break;
      }

      case SS_TRANSMIT: {
        Debugln( "got %c from %d", req.data, from_tid );
        CyclicBufferPush( &transmit_buffer, &req.data );
        if ( ! IsCyclicBufferFull( &transmit_buffer ) ) {
          Reply( transmitter_tid, NULL, 0 );
        }

        if ( waiting_on_transmit ) {
          char tmp;
          waiting_on_transmit = false;
          CyclicBufferPop( &transmit_buffer, &tmp );
          Debugln( "sending %c to courier", tmp );
          Reply( transmitted_courier_tid, &tmp, sizeof( tmp ) );
        }

        break;
      }

      default: break;
    }
  }
}
