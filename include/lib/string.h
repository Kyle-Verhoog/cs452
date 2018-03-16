#ifndef STRING_H
#define STRING_H

#ifndef X86
int strlen(const char *);
#else
#include <string.h>
#endif

int streq(const char *, const char *);
#endif
