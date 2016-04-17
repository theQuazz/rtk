#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdbool.h>
#include <stdint.h>

enum {
  TIMER_INTERRUPT_QUANTUM = 10000,
};

enum SystemEvent {
  TIMER_EXPIRED,
  CHAR_RECEIVED,
  CHAR_TRANSMIT,
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
char GetReceivedChar( void );
void TransmitChar( char c );
void TimersInit( void );
void UartsInit( void );

void bwputc( char c );
char bwgetc( void );

#endif
