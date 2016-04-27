#include "osacquire.h"

static struct Semaphore uart0_write_s = {
  .counter = 1,
  .blocked = {
    .last = NULL,
    .first = NULL,
  },
};

static struct Semaphore uart0_read_s = {
  .counter = 1,
  .blocked = {
    .first = NULL,
    .last = NULL,
  },
};

static struct Semaphore uart1_write_s = {
  .counter = 1,
  .blocked = {
    .first = NULL,
    .last = NULL,
  },
};

static struct Semaphore uart1_read_s = {
  .counter = 1,
  .blocked = {
    .first = NULL,
    .last = NULL,
  },
};

struct Semaphore *uar0_write = &uart0_write_s;
struct Semaphore *uar0_read = &uart0_read_s;
struct Semaphore *uar1_write = &uart1_write_s;
struct Semaphore *uar1_read = &uart1_read_s;

void OSAcquire( struct Semaphore *resource ) {
  SemV( resource );
}

void OSRelease( struct Semaphore *resource ) {
  SemP( resource );
}
