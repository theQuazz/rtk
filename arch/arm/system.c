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
  *( PIC + VIC_INTENABLE ) = intr;
}

uint32_t SystemEventToInterrupt( int eventid ) {
  switch ( eventid ) {
    case CHAR_RECEIVED:
    case CHAR_TRANSMIT:
      return PIC_UART0;
    default: return -1;
  }
}

int InterruptToSystemEvent( uint32_t intr ) {
  switch ( intr ) {
    case PIC_TIMER01: return TIMER_EXPIRED;
    case PIC_UART0: {
      Debugln( "PIC_UART0" );
      if ( !( *( UART0 + UARTFR ) & UARTFR_RXFE ) ) {
        *( UART0 + UARTICR ) = UARTICR_RXIC;
        return CHAR_RECEIVED;
      }
      if ( !( *( UART0 + UARTFR ) & UARTFR_TXFF ) ) {
        *( UART0 + UARTICR ) = UARTICR_TXIC;
        return CHAR_TRANSMIT;
      }
      return -1;
    }
    default: return -1;
  }
}

void EnableInterruptForEvent( int event ) {
  const int intr = SystemEventToInterrupt( event );
  if ( intr != -1 ) {
    Debugln( "ENABLING INTERRUPT 0x%08x", intr );
    *( PIC + VIC_INTENABLE ) = intr;
  }
}

char GetReceivedChar( int channel ) {
  switch ( channel ) {
    case 0: return *UART0;
    case 1: return *UART1;
    case 2: return *UART2;
    default: return '\0';
  }
}

void TransmitChar( int channel, char c ) {
  switch ( channel ) {
    case 0: *UART0 = c; return;
    case 1: *UART1 = c; return;
    case 2: *UART2 = c; return;
    default: return;
  }
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
  while ( *( UART1 + UARTFR ) & UARTFR_TXFF ) {}
  *UART1 = c;
}

char bwgetc( void ) {
  while ( *( UART1 + UARTFR ) & UARTFR_RXFE ) {}
  return *UART1;
}
