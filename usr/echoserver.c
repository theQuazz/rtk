#include "echoserver.h"
#include "../include/nameserver.h"
#include "../include/communication.h"
#include "../lib/string.h"
#include "../lib/print.h"

void EchoServer( void ) {
  while ( RegisterAs( "echo" ) == ERR_INVALID_NAMESERVER_TID );

  for ( ;; ) {
    char buf[128];
    int sender_tid;

    Receive( &sender_tid, buf, sizeof( buf ) );

    int offset = strcspn( buf, " " ) + 1;
    Print( 0, "%s\n", buf + offset );

    int ret = 0;
    Reply( sender_tid, &ret, sizeof( ret ) );
  }
}
