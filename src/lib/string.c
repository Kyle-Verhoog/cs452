#include <lib/string.h>

#ifndef X86
int strlen(const char *str) {
  int i;
  i = 0;
  while(*str++ != '\0') {
    i++;
  }
  return i;
}

#endif

int streq(const char *c, const char *s) {
  while (*c != '\0' && *s != '\0' && *c == *s && *c++ == *s++);
  return *c == *s && *c == '\0';
}
