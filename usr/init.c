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
#include "top.h"
#include "../include/task.h"
#include "../include/io.h"
#include "../include/clockserver.h"
#include "../lib/debug.h"
#include "../lib/math.h"

#include <stddef.h>

void Init( void ) {
  Debugln( "Initializing tasks..." );

  Create( HIGH_PRIORITY, NameServer );
  Create( HIGH_PRIORITY, ClockServer );
  Create( HIGH_PRIORITY, SerialInit );

  /* Shell Commands */
  Create( LOW_PRIORITY, WhoIsServer );
  Create( LOW_PRIORITY, EchoServer );
  Create( LOW_PRIORITY, RandServer );
  Create( LOW_PRIORITY, RpsServer );
  Create( LOW_PRIORITY, RpsCmdServer );
  Create( LOW_PRIORITY, TopServer );

  /* Shell */
  const int shell_tid = Create( LOW_PRIORITY, Shell );

  struct ShellConfig config = {
    prompt: "$ ",
    command_buffer_size: 128,
    channel: 0,
  };
  Send( shell_tid, &config, sizeof( config ), NULL, 0 );

  /* Test Tasks */
  // Create( MEDIUM_PRIORITY, RpsClient );
  // Create( MEDIUM_PRIORITY, RpsClient );
  // Create( MEDIUM_PRIORITY, RpsClient );
  // Create( MEDIUM_PRIORITY, RpsClient );
  // Create( MEDIUM_PRIORITY, usr1 );
  // Create( MEDIUM_PRIORITY, usr2 );
  // Create( MEDIUM_PRIORITY, usr3 );
  // Create( MEDIUM_PRIORITY, usr4 );

  Exit();
}
