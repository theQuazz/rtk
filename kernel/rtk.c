#include "rtk.h"
#include "../arch/arm/mmio.h"
#include "kernel.h"
#include "asm.h"
#include "task_utils.h"
#include "../usr/init.h"

void RtkInit(void) {
  tasks_init();
}

void RtkLaunch(void) {
  Task_create( LOW_PRIORITY, Init );

  for ( ;; ) {
    for ( int j = 0; j < 600000000; j++ ) { j = j + 1; }
    release_processor();
  }
}
