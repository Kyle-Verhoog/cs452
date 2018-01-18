#include <system.h>


/**
 * Prints out an error message and aborts the kernel.
 */
void kpanic(const char *fmt, ...) {
  va_list va;

  va_start(va,fmt);
  bwformat(LOG_COM, fmt, va);
  va_end(va);
  KABORT();
}
