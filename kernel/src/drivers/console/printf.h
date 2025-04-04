#include <stdarg.h>

void printf_internal(void (*putcFunc)(char), const char* fmt, va_list args);

