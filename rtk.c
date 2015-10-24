#include "rtk.h"
#include "memory.h"
#include "task.h"
#include "mmio.h"
#include "asm.h"

void user1(void) {
  for (int i = 0; ; i++) {
    uart_puts("user mode 1,");
    uart_putc('0' + i);
    uart_puts("!\n");
    for (int j = 0; j < 100000000; j++) {}
    syscall();
  }
}

void user2(void) {
  for (int i = 0; ; i++) {
    uart_puts("user mode 2,");
    uart_putc('0' + i);
    uart_puts("!\n");
    for (int j = 0; j < 100000000; j++) {}
    syscall();
  }
}

void user3(void) {
  for (int i = 0; ; i++) {
    uart_puts("user mode 3,");
    uart_putc('0' + i);
    uart_puts("!\n");
    for (int j = 0; j < 100000000; j++) {}
    syscall();
  }
}

void user4(void) {
  for (int i = 0; ; i++) {
    uart_puts("user mode 4,");
    uart_putc('0' + i);
    uart_puts("!\n");
    for (int j = 0; j < 100000000; j++) {}
    syscall();
  }
}

void rtk_init(void) {
  //uart_init();
  uart_puts("Kernel starting...\n");
  memory_init();
  uart_puts("memory initialized...\n");
  tasks_init();
  uart_puts("tasks initialized...\n");
}

void rtk_launch(void) {
  Task_create(20, user1);
  Task_create(20, user2);
  Task_create(20, user3);
  Task_create(20, user4);

  for ( ;; ) {
    release_processor();
  }

  uart_puts("Kernel stopping...\n");
}
