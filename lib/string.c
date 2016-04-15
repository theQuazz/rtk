#include "string.h"

#include <stddef.h>

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

char *strncpy( char *dest, const char *src, size_t n ) {
  char *dst = dest;

  while ( n-- ) {
    *dst++ = *src++;
  }

  if ( n ) {
    *dst = '\0';
  }

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

int strcmp( const char *s, const char *t ) {
  while ( *s && *s == *t ) {
    s++, t++;
  }

  return *( const unsigned char* )s - *( const unsigned char* )t;
}

int strncmp( const char *s, const char *t, size_t n ) {
  for ( ; n--; ++s, ++t ) {
    if ( *s != *t ) {
      return *s - *t;
    }
  }

  return 0;
}

void *memmove( void *dest, const void *src, size_t n ) {
  char *d = dest;
  const char *s = src;

  if ( d == s ) {
    return d;
  }

  if ( s < d && d < s + n ) {
    s += n;
    d += n;
    while ( n-- > 0 ) {
      *--d = *--s;
    }
  } else {
    memcpy( d, s, n );
  }

  return d;
}

char *strtok( char *str, const char *delim ) {
  static char *prev;
  size_t delim_len = strlen( delim );

  if ( str == NULL ) {
    str = prev;
  }

  char *start = str;

  while ( *str ) {
    if ( strncmp( delim, str, delim_len - 1 ) == 0 ) {
      *str = '\0';
      prev = str + 1;
      return start;
    }
    str++;
  }

  prev = NULL;

  return start;
}

const void *memchr( const void *ptr, int value, size_t num ) {
  const char *cptr = ptr;

  for ( ; --num; cptr++ ) {
    if ( *cptr == value ) {
      return cptr;
    }
  }

  return NULL;
}

const void *strchr( const char *str, int value ) {
  return memchr( str, value, strlen( str ) );
}

size_t strcspn( const char *str1, const char *str2 ) {
  size_t str2_len = strlen( str2 );
  size_t i;

  for ( i = 0; str1[i]; i++ ) {
    for ( size_t j = 0; j < str2_len; j++ ) {
      if ( str1[i] == str2[j] ) {
        return i;
      }
    }
  }

  return i;
}
