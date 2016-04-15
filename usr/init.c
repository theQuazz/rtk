#include "init.h"
#include "nameserver.h"
#include "shell.h"
#include "echoserver.h"
#include "whoisserver.h"
#include "randserver.h"
#include "test_procs.h"
#include "../include/task.h"
#include "../lib/debug.h"

#include <stddef.h>

void Init( void ) {
  Debugln( "Initializing tasks..." );

  Create( MEDIUM_PRIORITY, WhoIsServer );
  Create( MEDIUM_PRIORITY, EchoServer );
  Create( MEDIUM_PRIORITY, RandServer );
  Create( MEDIUM_PRIORITY, NameServer );
  const int shell_tid = Create( LOW_PRIORITY, Shell );

  struct ShellConfig config = {
    prompt: "$ ",
    command_buffer_size: 128,
  };

  Send( shell_tid, &config, sizeof( config ), NULL, 0 );
}
