#include "rtk.h"
#include "task.h"
#include "communication.h"
#include "../usr/null.h"
#include "../usr/init.h"

void RtkLaunch(void) {
  /* Initialize Modules */
  InitCommunication();

  /* Initialize Null & Init Tasks */
  CreateTask( NULL_PRIORITY, NullProcess );
  CreateTask( HIGHEST_PRIORITY, Init );

  /* Kernel Run Loop */
  for ( ;; ) {
    ScheduleAndActivate();
  }
}
