#include "null.h"
#include "../include/task.h"
#include "../lib/debug.h"
#include "../lib/util.h"

void NullProcess( void ) {
  for ( ;; ) {
    Debugln( "Null process..." );
    BusyDelayMs( 100 );
    Pass();
  }
}
