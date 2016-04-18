#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdarg.h>

#include "./print.h"

#ifdef DEBUG
#define Debugln( msg, ...) Debug( msg "\r\n", ##__VA_ARGS__ )
#define Debug( msg, ...) Print( 0, "[DEBUG] %s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__ )
#define DebugPrint(...) Print( 0, ##__VA_ARGS__ )
#else
#define Debugln(...)
#define Debug(...)
#define DebugPrint(...)
#endif

#define Error( msg, ...) Print( 0, "[ERROR] %s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__ )
#define Errorln( msg, ...) Error( msg "\n", ##__VA_ARGS__ )
#define Warn( msg, ...) Print( 0, "[WARN] %s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__ )
#define Warnln( msg, ...) Warn( msg "\n", ##__VA_ARGS__ )
#define Info( msg, ...) Print( 0, "[INFO] %s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__ )
#define Infoln( msg, ...) Info( msg "\n", ##__VA_ARGS__ )

#endif
