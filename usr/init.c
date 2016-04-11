#include "./init.h"
#include "../kernel/task.h"
#include "../kernel/task_utils.h"
#include "./nameserver.h"
#include "./test_procs.h"
#include "../lib/print.h"

void Init( void ) {
  // Task_create( LOW_PRIORITY, NameServer );

  Task_create( LOW_PRIORITY, usr1 );
  Task_create( LOW_PRIORITY, usr2 );
  Task_create( LOW_PRIORITY, usr3 );
  Task_create( LOW_PRIORITY, usr4 );

  for ( ;; ) {
    Print( 0, "Init...\n");
    syscall();
  }
}
