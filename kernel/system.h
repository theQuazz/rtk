#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdbool.h>
#include <stdint.h>

enum {
  TIMER_INTERRUPT_QUANTUM = 10000,
};

void SystemInit( void );
void Activate( uint32_t spsr, uint32_t *sp, uint32_t code );
bool IsTimerInterrupt( uint32_t intr );
bool TimerWentOff( void );
void ClearTimerInterrupt( void );
void DisableInterrupt( uint32_t intr );
uint32_t GetTimerTime( void );
void EnableInterrupt( int intr );
void TimersInit( void );

#endif
