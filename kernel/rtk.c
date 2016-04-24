#include "rtk.h"
#include "task.h"
#include "communication.h"
#include "system.h"
#include "../usr/null.h"
#include "../usr/init.h"

void RtkLaunch(void) {
  /* Run Architecture Spefic Initialization */
  SystemInit();

  /* Initialize Modules */
  InitTasks();
  InitCommunication();

  /* Initialize Null & Init Tasks */
  CreateTask( NULL_PRIORITY, NullProcess );
  CreateTask( LOW_PRIORITY, Init );

  /* Kernel Run Loop */
  for ( ;; ) {
    ScheduleAndActivate();
  }
}
