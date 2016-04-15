#include "random.h"
#include "debug.h"

static uint32_t seed_ = 362436069;
static int rand_errno = 0;

void Seed( uint32_t s ) {
  if ( seed_ == 0 ) {
    rand_errno = -1;
    return;
  }
  seed_ = s;
}

uint32_t GetSeed( void ) {
  return seed_;
}

uint32_t Rand( void ) {
  rand_errno = 0;
  seed_ = 36969 * (seed_ & 65535) + (seed_ >> 16);
  return seed_;
}

uint32_t RandMax( uint32_t max ) {
  if ( max == ( uint32_t ) -1 ) {
    rand_errno = -1;
    return 0;
  }
  return Rand() % ( max + 1 );
}

uint32_t RandRange( uint32_t min, uint32_t max ) {
  if ( min > max ) {
    rand_errno = -1;
    return 0;
  }
  return RandMax( max - min ) + min;
}

int RandErrno( void ) {
  return rand_errno;
}
