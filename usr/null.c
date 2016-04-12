#include "./null.h"
#include "../kernel/task_utils.h"
#include "../lib/print.h"

void NullProcess( void ) {
  for ( ;; ) {
    Print( 0, "Null process...\n" );
    Pass();
  }
}
