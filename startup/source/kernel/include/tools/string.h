#ifndef STRING_H
#define STRING_H

#include "tools/stdarg.h"

int strlen(const char* str);

// 仅仅支持 %d %c %s
int vsprintf(char* buf, const char* fmt, va_list args);

#endif  //