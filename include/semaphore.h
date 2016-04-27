#ifndef __I_SEMAPHORE_H__
#define __I_SEMAPHORE_H__

#include "../lib/queue.h"

struct Semaphore {
  int counter;
  struct queue blocked;
};

void SemInit( struct Semaphore *s, int initial );
void SemV( struct Semaphore *s );
void SemP( struct Semaphore *s );

#endif
