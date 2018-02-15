#ifndef IO_H
#define IO_H

#ifdef X86

#include <stdio.h>

#define PRINTF(...) printf(__VA_ARGS__)

#else

#include <defines.h>
#include <kernel/bwio.h>
#include <user/uio.h>

// tid_t UART2_TX;
#define PRINTF(...) bwprintf(LOG_COM, __VA_ARGS__)

#endif

#endif /* IO_H */
