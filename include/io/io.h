#ifndef IO_H
#define IO_H

#ifdef X86

#include <stdio.h>

#define PRINTF(...) printf(__VA_ARGS__)

#else

#include <defines.h>
#include <io/bwio.h>

#define PRINTF(...) bwprintf(LOG_COM, __VA_ARGS__)

#endif

#endif /* IO_H */
