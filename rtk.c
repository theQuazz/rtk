#include "rtk.h"
#include "memory.h"
#include "task.h"

void rtk_init(void) {
  memory_init();
  tasks_init();
}

void rtk_launch(void) {
  release_processor();
}
