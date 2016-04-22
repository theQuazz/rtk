#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

size_t strlen( const char* str );
size_t strnlen( const char* str, size_t maxlen );
char *strcpy( char *dest, const char *src );
char *strncpy( char *dest, const char *src, size_t num );
void *memcpy( void *dest, const void *src, size_t num );
int strncmp( const char *s, const char *t, size_t n );
int strcmp( const char *s, const char *t );
void *memmove( void *dest, const void *src, size_t n );
char *strtok( char *str, const char *delim );
const void *memchr( const void *ptr, int value, size_t num );
const void *strchr( const char *str, int value );
size_t strcspn( const char *str1, const char *str2 );

#endif
