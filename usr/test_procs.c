#include "test_procs.h"
#include "../include/task.h"
#include "../include/communication.h"
#include "../include/nameserver.h"
#include "../include/clockserver.h"
#include "../lib/print.h"
#include "../lib/util.h"

void usr1() {
  for ( ;; ) {
    int time = Time();
    Print( 0, "usr1! %02d:%02d:%02d.%01d\r", ( time / 360000 ) % 60, ( time / 6000 ) % 60, ( time / 100 ) % 60, ( time / 10) % 10 );
    Delay( 10 );
  }
}

void usr2() {
  char msg[] = "Test message usr1 MORE STUFF MORE STUFF MORE STUFF";
  char buf[20];

  Print( 0, "usr2: MyTid() => %d\n", MyTid() );
  Print( 0, "usr2: RegisterAs( \"rtk.usr.test_procs.usr1\" ) => %d\n", RegisterAs( "rtk.usr.test_procs.usr1" ) );
  Print( 0, "usr2: RegisterAs( \"rtk.usr.test_procs.usr2\" ) => %d\n", RegisterAs( "rtk.usr.test_procs.usr2" ) );
  Print( 0, "usr2: WhoIs( \"rtk.usr.test_procs.usr2\" ) => %d\n", WhoIs( "rtk.usr.test_procs.usr2" ) );
  Print( 0, "usr2: WhoIs( \"rtk.usr.test_procs.usr3\" ) => %d\n", WhoIs( "rtk.usr.test_procs.usr3" ) );

  Print( 0, "usr2: Send( -1, %d, %d, %d, %d ) => err( %d )\n", msg, sizeof( msg ), buf, sizeof( buf ), Send( -1, msg, sizeof( msg ), buf, sizeof( buf ) ) );
  Print( 0, "usr2: Send( TASK_MAX, %d, %d, %d, %d ) => err( %d )\n", msg, sizeof( msg ), buf, sizeof( buf ), Send( 1024, msg, sizeof( msg ), buf, sizeof( buf ) ) );
  Print( 0, "usr2: Send( Zombied, %d, %d, %d, %d ) => err( %d )\n", msg, sizeof( msg ), buf, sizeof( buf ), Send( 1, msg, sizeof( msg ), buf, sizeof( buf ) ) );
  Print( 0, "usr2: Send( NotStarted, %d, %d, %d, %d ) => err( %d )\n", msg, sizeof( msg ), buf, sizeof( buf ), Send( 7, msg, sizeof( msg ), buf, sizeof( buf ) ) );
  Print( 0, "usr2: Reply( -1, %d, %d ) => err( %d )\n", msg, sizeof( msg ), Reply( -1, msg, sizeof( msg ) ) );
  Print( 0, "usr2: Reply( TASK_MAX, %d, %d ) => err( %d )\n", msg, sizeof( msg ), Reply( 1024, msg, sizeof( msg ) ) );
  Print( 0, "usr2: Reply( Zombied, %d, %d ) => err( %d )\n", msg, sizeof( msg ), Reply( 1, msg, sizeof( msg ) ) );
  Print( 0, "usr2: Reply( NotStarted, %d, %d ) => err( %d )\n", msg, sizeof( msg ), Reply( 7, msg, sizeof( msg ) ) );
  Print( 0, "usr2: Reply( TooSmallBuffer, %d, %d ) => err( %d )\n", msg, sizeof( msg ), Reply( 5, msg, sizeof( msg ) ) );
  Print( 0, "usr2: Reply( NotReplyBlocked, %d, %d ) => err( %d )\n", msg, sizeof( msg ), Reply( 4, msg, sizeof( msg ) ) );
}

void usr3() {
  char msg[20];
  int fromtid;

  Print( 0, "usr3: starting\n" );

  for ( int i = 0; i < 2; i += 1 ) {
    const int msg_size = Receive( &fromtid, msg, sizeof( msg ) );

    Print( 0, "usr3: %d said \"%s\" (%d)\n", fromtid, msg, msg_size );

    char reply[] = "Thanks!";
    const int err = Reply( fromtid, reply, sizeof( reply ) );

    Print( 0, "usr3: err( %d )\n", err );

    BusyDelayMs( 100 );
    Pass();
  }
}

void usr4() {
  char msg[] = "Hi from usr4!";
  char reply[20];

  Print( 0, "usr4: starting\n" );

  for ( int i = 0; i < 2; i += 1 ) {
    const int reply_size = Send( 4, msg, sizeof( msg ), reply, 20 );

    Print( 0, "usr4: got back \"%s\" (%d)\n", reply, reply_size );

    BusyDelayMs( 100 );
    Pass();
  }
}
