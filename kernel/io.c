#include "io.h"
#include "../include/common.h"
#include "../arch/arm/mmio.h"

int GetcProxy( int channel ) {
  return uart_getc();
}

int PutcProxy( int channel, char ch ) {
  uart_putc( ch );
  return RETURN_STATUS_OK;
}
