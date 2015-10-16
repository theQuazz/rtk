#include "rtk.h"
#include "memory.h"
#include "task.h"
#include "vga.h"

void rtk_init(void) {
  //memory_init();
  //tasks_init();
  terminal_init();
}

void rtk_launch(void) {
  terminal_writestring("Kernel up...");
  //release_processor();
}
