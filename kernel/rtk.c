#include "rtk.h"
#include "task.h"
#include "../usr/null.h"
#include "../usr/init.h"

void RtkLaunch(void) {
  CreateTask( NULL_PRIORITY, NullProcess );
  CreateTask( HIGHEST_PRIORITY, Init );

  for ( ;; ) {
    ScheduleAndActivate();
  }
}
