#include "versatilepb.h"
#include "../../kernel/system.h"
#include "../../include/interrupt.h"

#include <stdbool.h>
#include <stdint.h>

void SystemInit( void ) {
  TimersInit();
  //UartsInit();
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
  *( PIC + VIC_INTENCLEAR ) = intr;
}

uint32_t SystemEventToInterrupt( int eventid ) {
  switch ( eventid ) {
    case CHAR_RECEIVED:
    case CHAR_TRANSMIT:
      return ( uint32_t )UART0;
    default: return -1;
  }
}

int InterruptToSystemEvent( uint32_t intr ) {
  switch ( intr ) {
    case PIC_TIMER01: return TIMER_EXPIRED;
    case PIC_UART0: {
      if ( !( *( UART0 + UARTFR ) & UARTFR_TXFF ) ) {
        return CHAR_TRANSMIT;
      }
      else if ( !( *( UART0 + UARTFR ) & UARTFR_RXFE ) ) {
        return CHAR_RECEIVED;
      }
      return -1;
    }
    default: return -1;
  }
}

void EnableInterruptForEvent( int event ) {
  const int intr = SystemEventToInterrupt( event );
  if ( intr != -1 ) {
    *( PIC + VIC_INTENABLE ) = intr;
  }
}

char GetReceivedChar( void ) {
  return *UART0;
}

void TransmitChar( char c ) {
  *UART0 = c;
}

void UartsInit( void ) {
  *( UART0 + UARTIMSC ) |= UARTIMSC_RXIM;
  *( UART0 + UARTIMSC ) |= UARTIMSC_TXIM;
}

void TimersInit( void ) {
  *( PIC + VIC_INTENABLE ) = PIC_TIMER01;
  *TIMER0 = TIMER_INTERRUPT_QUANTUM;
  *( TIMER0 + TIMER_CONTROL ) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT | TIMER_INTEN;
}

void bwputc( char c ) {
  while ( *( UART0 + UARTFR ) & UARTFR_TXFF ) {}
  *UART0 = c;
}

char bwgetc( void ) {
  while ( *( UART0 + UARTFR ) & UARTFR_RXFE ) {}
  return *UART0;
}
