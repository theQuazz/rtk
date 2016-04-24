#include "whoisserver.h"
#include "../include/nameserver.h"
#include "../include/communication.h"
#include "../lib/string.h"
#include "../lib/print.h"

#include <stddef.h>

void WhoIsServer( void ) {
  while ( RegisterAs( "whois" ) == ERR_INVALID_NAMESERVER_TID );

  for ( ;; ) {
    char buf[128];
    int sender_tid;

    Receive( &sender_tid, buf, sizeof( buf ) );

    strtok( buf, " " );
    int tid = WhoIs( strtok( NULL, " " ) );
    int ret = 0;

    if ( tid >= 0 ) {
      Print( 0, "%d\r\n", tid );
    }
    else {
      ret = tid;
    }

    Reply( sender_tid, &ret, sizeof( ret ) );
  }
}
