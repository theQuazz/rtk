#include "string.h"

size_t strlen(const char *str) {
  size_t ret = 0;
  while (str[ret++]);
  return ret;
}

void *memcpy(void *dest, void *src, size_t num) {
  char *d = dest, *s = src;
  while (--num >= 0) d[num] = s[num];
  return dest;
}
