#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

size_t strlen( const char* str );
size_t strnlen( const char* str, size_t maxlen );
char *strncpy( char *dest, const char *src, size_t num );
void *memcpy( void *dest, const void *src, size_t num );

#endif
