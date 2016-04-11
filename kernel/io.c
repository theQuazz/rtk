#include "./io.h"
#include "../arch/arm/mmio.h"

int Getc( int channel ) {
  return uart_getc();
}

int Putc( int channel, char ch ) {
  uart_putc( ch );
  return RETURN_STATUS_OK;
}
