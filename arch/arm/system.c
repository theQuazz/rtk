#include "versatilepb.h"
#include "../../kernel/system.h"
#include "../../include/interrupt.h"
#include "../../lib/debug.h"

#include <stdbool.h>
#include <stdint.h>

void SystemInit( void ) {
  TimersInit();
  UartsInit();
}

bool IsTimerInterrupt( uint32_t intr ) {
  return intr == PIC_TIMER01;
}

bool TimerWentOff( void ) {
  return *( TIMER0 + TIMER_MIS );
}

void ClearTimerInterrupt( void ) {
  *( TIMER0 + TIMER_INTCLR ) = 1;
}

uint32_t GetTimerTime( void ) {
  return *( TIMER0 + TIMER_VALUE );
}

void DisableInterrupt( uint32_t intr ) {
  Debugln( "DISABLING INTERRUPT 0x%08x", intr );
  *( PIC + VIC_INTENCLEAR ) = intr;
}

void EnableInterrupt( int intr ) {
  Debugln( "ENABLING INTERRUPT 0x%08x", intr );
  *( PIC + VIC_INTENABLE ) = intr;
}
void UartsInit( void ) {
}

void TimersInit( void ) {
  *( PIC + VIC_INTENABLE ) = PIC_TIMER01;
  *TIMER0 = TIMER_INTERRUPT_QUANTUM;
  *( TIMER0 + TIMER_CONTROL ) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT | TIMER_INTEN;
}

void bwputc( char c ) {
  while ( *( UART1 + UARTFR ) & UARTFR_TXFF ) {}
  *UART1 = c;
}

char bwgetc( void ) {
  while ( *( UART1 + UARTFR ) & UARTFR_RXFE ) {}
  return *UART1;
}
