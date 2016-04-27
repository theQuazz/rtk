#ifndef __OSACQUIRE_H__
#define __OSACQUIRE_H__

#include "../include/semaphore.h"

extern struct Semaphore *uart0_write, *uart0_read, *uart1_write, *uart1_read;

void OSAcquire( struct Semaphore *resouce );

#endif
