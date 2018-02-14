#ifndef IOSERVER_H
#define IOSERVER_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>
#include <user/clockserver.h>
#include <io/io.h>
#include <debug/debug.h>
#include <lib/circular_buffer.h>

typedef struct IONotifierArgs {
  int inter;
  int tid;
  int uart;
  int emask;
} IONotifierArgs;

typedef struct IOServerArgs {
  int uart_id;
  int flags;
  tid_t parent_tid;
} IOServerArgs;

typedef enum IOServerRequests {
  IO_GETC = 0,
  IO_PUTC = 1,
} IOServerReq;

/*
typedef struct IOServerNotifierRequest {
  int inttype;
} IOSNReq;
*/

void IOServerUART2();

char GetC(tid_t ios_tid);

CIRCULAR_BUFFER_DEC(io_cb, char, 512);

#endif
