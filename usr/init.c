#include "init.h"
#include "nameserver.h"
#include "shell.h"
#include "echoserver.h"
#include "whoisserver.h"
#include "randserver.h"
#include "rps.h"
#include "clockserver.h"
#include "serialserver.h"
#include "test_procs.h"
#include "../include/task.h"
#include "../include/io.h"
#include "../include/clockserver.h"
#include "../lib/debug.h"

#include <stddef.h>

void SsTest( void ) {
  for ( int i = 0; ; i++ ) {
    Print( 0, "%d:%c ", i, Getc( 0 ) );
  }
}

void Init( void ) {
  Debugln( "Initializing tasks..." );

  Create( HIGH_PRIORITY, NameServer );
  // Create( MEDIUM_PRIORITY, WhoIsServer );
  // Create( MEDIUM_PRIORITY, EchoServer );
  // Create( MEDIUM_PRIORITY, RandServer );
  // Create( MEDIUM_PRIORITY, RpsServer );
  // Create( MEDIUM_PRIORITY, RpsCmdServer );
  // Create( MEDIUM_PRIORITY, RpsClient );
  // Create( MEDIUM_PRIORITY, RpsClient );
  // Create( MEDIUM_PRIORITY, RpsClient );
  // Create( MEDIUM_PRIORITY, RpsClient );
  Create( HIGH_PRIORITY, ClockServer );
  // Create( MEDIUM_PRIORITY, usr1 );
  Create( HIGH_PRIORITY, SerialInit );
  Create( LOW_PRIORITY, SsTest );
  //Create( HIGHEST_PRIORITY, SsTest );
  //Create( HIGHEST_PRIORITY, SsTest );
  //Create( HIGHEST_PRIORITY, SsTest );
  //Create( HIGHEST_PRIORITY, SsTest );
  //Create( HIGHEST_PRIORITY, SsTest );
  //Create( HIGHEST_PRIORITY, SsTest );
  //Create( HIGHEST_PRIORITY, SsTest );

  //const int shell_tid = Create( LOW_PRIORITY, Shell );

  //struct ShellConfig config = {
  //  prompt: "$ ",
  //  command_buffer_size: 128,
  //};

  //Send( shell_tid, &config, sizeof( config ), NULL, 0 );
}
