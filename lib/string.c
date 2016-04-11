#include "string.h"

size_t strlen( const char *str ) {
  size_t ret = 0;
  while ( str[ret++] );
  return ret;
}

size_t strnlen( const char *str, size_t num ) {
  size_t ret = 0;
  while ( str[ret++] && num-- );
  return ret;
}

char *strncpy( char *dest, const char *src, size_t num ) {
  size_t i = 0;
  while ( src[i++] && num-- ) dest[i] = src[i];
  dest[i] = '\0';
  return dest;
}

void *memcpy( void *dest, const void *src, size_t num ) {
  char* dst8 = ( char* )dest;
  char* src8 = ( char* )src;

  while ( num-- ) {
    *dst8++ = *src8++;
  }

  return dest;
}
