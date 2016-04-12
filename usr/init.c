#include "./init.h"
#include "../kernel/task.h"
#include "../kernel/task_utils.h"
#include "./nameserver.h"
#include "./test_procs.h"
#include "../lib/debug.h"

void Init( void ) {
  Debugln( "Initializing tasks..." );

  Create( LOW_PRIORITY, usr1 );
  Create( LOW_PRIORITY, usr2 );
  Create( LOW_PRIORITY, usr3 );
  Create( LOW_PRIORITY, usr4 );
}
