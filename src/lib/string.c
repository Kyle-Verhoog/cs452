#include <lib/string.h>


int strlen(const char *str) {
  int i;
  i = 0;
  while(*str++ != '\0') {
    i++;
  }
  return i;
}
