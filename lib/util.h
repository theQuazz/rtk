#ifndef __UTIL_H__
#define __UTIL_H__

#define BusyDelayMs( ms ) for ( long long ddd = 0; ddd / 100 < ms * 2400; ddd++ ) asm("")

#endif
