#include "io.h"
#include "system.h"
#include "../include/common.h"

int GetcProxy( int channel ) {
  return bwgetc();
}

int PutcProxy( int channel, char ch ) {
  bwputc( ch );
  return RETURN_STATUS_OK;
}
