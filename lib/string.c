#include "string.h"

size_t strlen(const char *str) {
  size_t ret = 0;
  while (str[ret++]);
  return ret;
}

void *memcpy(void *dest, const void *src, size_t num) {
  char* dst8 = (char*)dest;
  char* src8 = (char*)src;

  while (num--) {
    *dst8++ = *src8++;
  }

  return dest;
}
