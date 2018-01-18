#include <system.h>

void kassert_panic(const char *fmt, ...) {
  va_list va;

  va_start(va,fmt);
  bwformat(LOG_COM, fmt, va);
  va_end(va);
  __asm__("");
}
