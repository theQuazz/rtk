#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdbool.h>
#include <stdint.h>

enum {
  TIMER_INTERRUPT_QUANTUM = 10000000,
};

void SystemInit( void );
void Activate( uint32_t spsr, uint32_t *sp, uint32_t code );
bool IsTimerInterrupt( uint32_t intr );
bool TimerWentOff( void );
void ClearTimerInterrupt( void );
void DisableInterrupt( uint32_t intr );
uint32_t GetTimerTime( void );
int InterruptToSystemEvent( uint32_t intr );
void EnableInterruptForEvent( int event );
char GetReceivedChar( int channel );
void TransmitChar( int channel, char c );
void TimersInit( void );
void UartsInit( void );

void bwputc( char c );
char bwgetc( void );

#endif
