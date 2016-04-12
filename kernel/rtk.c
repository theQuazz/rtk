#include "rtk.h"
#include "../arch/arm/mmio.h"
#include "kernel.h"
#include "asm.h"
#include "task_utils.h"
#include "../usr/null.h"
#include "../usr/init.h"

void RtkLaunch(void) {
  CreateTask( NULL_PRIORITY, NullProcess );
  CreateTask( HIGHEST_PRIORITY, Init );

  for ( ;; ) {
    ScheduleAndActivate();
  }
}
