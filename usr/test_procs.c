#include "test_procs.h"
#include "../include/task.h"
#include "../include/communication.h"
#include "../lib/print.h"
#include "../lib/util.h"

void usr1() {
  char msg[] = "Hi from usr1!";
  char reply[20];

  Print( 0, "usr1: starting\n" );

  for ( int i = 0; ; i += 4 ) {
    const int reply_size = Send( 3, msg, sizeof( msg ), reply, 20 );

    Print( 0, "usr1: got back \"%s\" (%d)\n", reply, reply_size );

    DelaySeconds( 1 );
    Pass();
  }
}

void usr2() {
  char msg[20];
  int fromtid;

  Print( 0, "usr2: starting\n" );

  for ( int i = 1; ; i += 4 ) {
    const int msg_size = Receive( &fromtid, msg, sizeof( msg ) );

    Print( 0, "usr2: %d said \"%s\" (%d)\n", fromtid, msg, msg_size );

    char reply[] = "Thanks!";
    const int err = Reply( fromtid, reply, sizeof( reply ) );

    Print( 0, "usr2: err( %d )\n", err );

    DelaySeconds( 1 );
    Pass();
  }
}

void usr3() {
  char msg[20];
  int fromtid;

  Print( 0, "usr3: starting\n" );

  for ( int i = 2; ; i += 4 ) {
    const int msg_size = Receive( &fromtid, msg, sizeof( msg ) );

    Print( 0, "usr3: %d said \"%s\" (%d)\n", fromtid, msg, msg_size );

    char reply[] = "Thanks!";
    const int err = Reply( fromtid, reply, sizeof( reply ) );

    Print( 0, "usr3: err( %d )\n", err );

    DelaySeconds( 1 );
    Pass();
  }
}

void usr4() {
  char msg[] = "Hi from usr4!";
  char reply[20];

  Print( 0, "usr4: starting\n" );

  for ( int i = 3; ; i += 4 ) {
    const int reply_size = Send( 4, msg, sizeof( msg ), reply, 20 );

    Print( 0, "usr4: got back \"%s\" (%d)\n", reply, reply_size );

    DelaySeconds( 1 );
    Pass();
  }
}
