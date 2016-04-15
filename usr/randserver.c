#include "echoserver.h"
#include "../include/nameserver.h"
#include "../include/communication.h"
#include "../lib/string.h"
#include "../lib/print.h"
#include "../lib/random.h"
#include "../lib/stdlib.h"

void RandServer( void ) {
  while ( RegisterAs( "rand" ) == ERR_INVALID_NAMESERVER_TID );

  for ( ;; ) {
    char buf[128];
    int sender_tid;

    Receive( &sender_tid, buf, sizeof( buf ) );

    strtok( buf, " " );
    char *fst = strtok( NULL, " " );
    char *lst = fst != NULL ? strtok( NULL, " " ) : NULL;

    int randn;
    if ( fst && lst ) {
      randn = RandRange( atoi( fst ), atoi( lst ) );
    }
    else if ( fst ) {
      randn = RandMax( atoi( fst ) );
    }
    else {
      randn = Rand();
    }

    int errno = RandErrno();

    if ( errno == 0 ) {
      Print( 0, "%d\n", randn );
    }

    Reply( sender_tid, &errno, sizeof( errno ) );
  }
}
