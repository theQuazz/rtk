#include "serialserver.h"
#include "../include/task.h"
#include "../include/nameserver.h"
#include "../include/io.h"
#include "../include/interrupt.h"
#include "../include/communication.h"
#include "../include/clockserver.h"
#include "../lib/cyclicbuffer.h"
#include "../lib/debug.h"
#include "../lib/string.h"
#include "../arch/arm/versatilepb.h"

#include <stddef.h>

enum SerialConfigType {
  SS_CONFIG,
};

struct SerialConfig {
  struct BaseMsg;
  unsigned int pic_reg;
  volatile unsigned int *uart;
  unsigned int channel;
  int notifier_tid;
};

enum SerialRequestType {
  SS_NOTIFIER,
  SS_TRANSMIT,
  SS_RECEIVE,
};

struct SerialRequest {
  struct BaseMsg;
  char data;
};

static char TRANSMIT_FMT[] = "transmit::%d";
static char RECEIVE_FMT[] = "receive::%d";

static volatile unsigned int *uarts[] = {
  UART0,
  UART1,
  UART2,
};

int Putc( int channel, char ch ) {
  if ( channel == 2 ) {
    bwputc( uarts[channel], ch );
    return 0;
  }

  char whois[sizeof( TRANSMIT_FMT )];
  Fmt( whois, TRANSMIT_FMT, channel );

  const int serial_tid = WhoIs( whois );

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
  if ( channel == 2 ) {
    return bwgetc( uarts[channel] );
  }

  char whois[sizeof( RECEIVE_FMT )];
  Fmt( whois, RECEIVE_FMT, channel );

  const int serial_tid = WhoIs( whois );

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

void ReceiveSecretary( void ) {
  struct SerialConfig config;
  int server_tid;

  Receive( &server_tid, &config, sizeof( config ) );
  Reply( server_tid, NULL, 0 );

  char registration[sizeof( RECEIVE_FMT )];
  Fmt( registration, RECEIVE_FMT, config.channel );

  Debugln( "ReceiveSecretary registering as %s", registration );

  while ( RegisterAs( registration ) == ERR_INVALID_NAMESERVER_TID );

  for ( ;; ) {
    int sender_tid;
    struct SerialRequest req;
    char res;
    Receive( &sender_tid, &req, sizeof( req ) );
    Send( server_tid, &req, sizeof( req ), &res, sizeof( res ) );
    Reply( sender_tid, &res, sizeof( res ) );
  }
}

void TransmitSecretary( void ) {
  struct SerialConfig config;
  int server_tid;

  Receive( &server_tid, &config, sizeof( config ) );
  Reply( server_tid, NULL, 0 );

  char registration[sizeof( TRANSMIT_FMT )];
  Fmt( registration, TRANSMIT_FMT, config.channel );

  Debugln( "TransmitSecretary registering as %s", registration );

  while ( RegisterAs( registration ) == ERR_INVALID_NAMESERVER_TID );

  for ( ;; ) {
    int sender_tid;
    struct SerialRequest req;
    Receive( &sender_tid, &req, sizeof( req ) );
    Send( server_tid, &req, sizeof( req ), NULL, 0 );
    Reply( sender_tid, NULL, 0 );
  }
}

void Notifier( void ) {
  struct SerialConfig config;
  int courier;

  Receive( &courier, &config, sizeof( config ) );
  Reply( courier, NULL, 0 );

  *( config.uart + UARTIMSC ) |= UARTIMSC_RXIM;
  *( config.uart + UARTIMSC ) |= UARTIMSC_TXIM;

  for ( ;; ) {
    Receive( &courier, NULL, 0 );
    AwaitEvent( config.pic_reg );
    Reply( courier, NULL, 0 );
  }
}

void Courier( void ) {
  struct SerialConfig config;
  int server_tid;

  Receive( &server_tid, &config, sizeof( config ) );
  Send( config.notifier_tid, &config, sizeof( config ), NULL, 0 );
  Reply( server_tid, NULL, 0 );

  struct SerialRequest req = {
    .type = SS_NOTIFIER,
  };

  for ( ;; ) {
    Send( config.notifier_tid, NULL, 0, NULL, 0 );
    Send( server_tid, &req, sizeof( req ), NULL, 0 );
  }
}

void SerialServer( void ) {
  struct SerialConfig config;

  Debugln( "SerialServer starting" );

  int sender_tid;
  Receive( &sender_tid, &config, sizeof( config ) );
  Reply( sender_tid, NULL, 0 );

  int notifier_tid = Create( HIGHEST_PRIORITY, Notifier );
  int courier_tid = Create( HIGH_PRIORITY, Courier );
  int transmit_tid = Create( MEDIUM_PRIORITY, TransmitSecretary );
  int receive_tid = Create( MEDIUM_PRIORITY, ReceiveSecretary );

  config.notifier_tid = notifier_tid;
  Send( courier_tid, &config, sizeof( config ), NULL, 0 );
  Send( transmit_tid, &config, sizeof( config ), NULL, 0 );
  Send( receive_tid, &config, sizeof( config ), NULL, 0 );

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

  bool awaiting_receive = false;
  bool awaiting_transmit = false;

  Debugln( "SerialServer UART(0x%08x) ready", config.uart );

  for ( ;; ) {
    struct SerialRequest req;
    int from_tid;
    Receive( &from_tid, &req, sizeof( req ) );

    switch ( req.type ) {
      case SS_NOTIFIER: {
        Reply( courier_tid, NULL, 0 );

        if ( *( config.uart + UARTFR ) & UARTFR_RXFF ) { // RECEIVED
          *( config.uart + UARTICR ) = UARTICR_RXIC;
          char recv = *config.uart;

          if ( awaiting_receive ) {
            awaiting_receive = false;
            Reply( receive_tid, &recv, sizeof( recv ) );
          }
          else {
            CyclicBufferPush( &receive_buffer, &recv );
          }
        }

        if ( awaiting_transmit && *( config.uart + UARTFR ) & UARTFR_TXFE ) { // TRANSMITTED
          *( config.uart + UARTICR ) = UARTICR_TXIC;
          awaiting_transmit = false;

          if ( IsCyclicBufferEmpty( &transmit_buffer ) ) {
            awaiting_transmit = false;
          }
          else {
            bool was_full = IsCyclicBufferFull( &transmit_buffer );

            CyclicBufferPop( &transmit_buffer, ( char* )config.uart );

            if ( was_full ) {
              Reply( transmit_tid, NULL, 0 );
            }
          }
        }

        break;
      }

      case SS_RECEIVE: {
        if ( IsCyclicBufferEmpty( &receive_buffer ) ) {
          awaiting_receive = true;
        }
        else {
          char tmp;
          CyclicBufferPop( &receive_buffer, &tmp );
          Reply( receive_tid, &tmp, sizeof( tmp ) );
        }

        break;
      }

      case SS_TRANSMIT: {
        CyclicBufferPush( &transmit_buffer, &req.data );
        if ( ! IsCyclicBufferFull( &transmit_buffer ) ) {
          Reply( transmit_tid, NULL, 0 );
        }

        if ( ! awaiting_transmit ) {
          awaiting_transmit = true;
          CyclicBufferPop( &transmit_buffer, ( char* )UART0 );
        }

        break;
      }

      default: break;
    }
  }
}

void SerialInit( void ) {
  struct SerialConfig uart0 = {
    .type = SS_CONFIG,
    .pic_reg = PIC_UART0,
    .uart = UART0,
    .channel = 0,
  };
  int uart0_tid = Create( HIGH_PRIORITY, SerialServer );
  Send( uart0_tid, &uart0, sizeof( uart0 ), NULL, 0 );

  struct SerialConfig uart1 = {
    .type = SS_CONFIG,
    .pic_reg = PIC_UART1,
    .uart = UART1,
    .channel = 1,
  };
  int uart1_tid = Create( HIGH_PRIORITY, SerialServer );
  Send( uart1_tid, &uart1, sizeof( uart1 ), NULL, 0 );

  //struct SerialConfig uart2 = {
  //  .type = SS_CONFIG,
  //  .pic_reg = PIC_UART2,
  //  .uart = UART2,
  //  .channel = 2,
  //};
  //int uart2_tid = Create( HIGH_PRIORITY, SerialServer );
  //Send( uart2_tid, &uart2, sizeof( uart2 ), NULL, 0 );
}
