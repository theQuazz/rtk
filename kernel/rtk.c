#include "rtk.h"
#include "../arch/arm/mmio.h"
#include "kernel.h"
#include "asm.h"
#include "task_utils.h"

void user1(void) {
  for (int i = 0; ; i += 4) {
    uart_putc('0' + i);
    uart_putc('\n');
    for (int j = 0; j < 100000000; j++) {}
    syscall();
  }
}

void user2(void) {
  for (int i = 1; ; i += 4) {
    uart_putc('0' + i);
    uart_putc('\n');
    for (int j = 0; j < 100000000; j++) {}
    syscall();
  }
}

void user3(void) {
  for (int i = 2; ; i += 4) {
    uart_putc('0' + i);
    uart_putc('\n');
    for (int j = 0; j < 100000000; j++) {}
    syscall();
  }
}

void user4(void) {
  for (int i = 3; ; i += 4) {
    uart_putc('0' + i);
    uart_putc('\n');
    for (int j = 0; j < 100000000; j++) {}
    syscall();
  }
}

void rtk_init(void) {
}

void rtk_launch(void) {
  Task_create(HIGHEST_PRIORITY, user1);
  Task_create(HIGHEST_PRIORITY, user2);
  Task_create(HIGHEST_PRIORITY, user3);
  Task_create(HIGHEST_PRIORITY, user4);

  for ( ;; ) {
    release_processor();
  }
}
