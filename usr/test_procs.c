#include "test_procs.h"
#include "../include/task.h"
#include "../include/io.h"
#include "../lib/print.h"
#include "../lib/util.h"

void usr1() {
  for ( int i = 0; ; i += 4 ) {
    Print( 0, "usr1: %d\n", i );
    DelaySeconds( 1 );
    Pass();
  }
}

void usr2() {
  for ( int i = 1; ; i += 4 ) {
    Print( 0, "usr2: %d\n", i );
    DelaySeconds( 1 );
    Pass();
  }
}

void usr3() {
  for ( int i = 2; ; i += 4 ) {
    Print( 0, "usr3: %d\n", i );
    DelaySeconds( 1 );
    Pass();
  }
}

void usr4() {
  for ( int i = 3; ; i += 4 ) {
    Print( 0, "usr4: %d\n", i );
    DelaySeconds( 1 );
    Pass();
  }
}
