#ifndef IO_H
#define IO_H

#ifdef X86

#include <stdio.h>

#define PRINTF(...) printf(__VA_ARGS__)

#else

#include <io/bwio.h>

#define LOG_COM 1
#define PRINTF(...) bwprintf(LOG_COM, __VA_ARGS__)
#endif

#endif /* IO_H */
