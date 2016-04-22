#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdarg.h>

#include "./print.h"

#ifdef DEBUG
#define Debugln( msg, ...) Debug( msg "\r\n", ##__VA_ARGS__ )
#define Debug( msg, ...) Print( 2, "[DEBUG] %s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__ )
#define DebugPrint(...) Print( 2, ##__VA_ARGS__ )
#else
#define Debugln(...)
#define Debug(...)
#define DebugPrint(...)
#endif

#define Error( msg, ...) Print( 2, "[ERROR] %s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__ )
#define Errorln( msg, ...) Error( msg "\n", ##__VA_ARGS__ )
#define Warn( msg, ...) Print( 2, "[WARN] %s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__ )
#define Warnln( msg, ...) Warn( msg "\n", ##__VA_ARGS__ )
#define Info( msg, ...) Print( 2, "[INFO] %s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__ )
#define Infoln( msg, ...) Info( msg "\n", ##__VA_ARGS__ )

#endif
