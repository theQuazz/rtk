#include "null.h"
#include "../include/task.h"
#include "../lib/print.h"

void NullProcess( void ) {
  for ( ;; ) {
    Print( 0, "Null process...\n" );
    Pass();
  }
}
