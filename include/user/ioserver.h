#ifndef IOSERVER_H
#define IOSERVER_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>
#include <user/clockserver.h>
#include <user/uio.h>
#include <debug/debug.h>
#include <lib/circular_buffer.h>


typedef struct IOServerArgs {
  InterruptEvent ie_base;
  int uart_base;
  int ns_id;
  int flags;
} IOServerArgs;

typedef enum IOServerRequestType {
  IO_GETC = 0,
  IO_PUTC = 1,
  IO_TX   = 2,
  IO_RX   = 3,
  IO_RT   = 4,
  IO_MI   = 5,
} IOServerReqType;

typedef struct IONotifierArgs {
  IOServerReqType type;
  InterruptEvent ie;
  int tid;
  int uart;
  int emask;
} IONotifierArgs;

typedef struct IOServerRequest {
  char *msg;
  int   len;
  IOServerReqType type; // Note: the type must be the last (first) element!
} IOServerReq;

void IOServerUART2();

char GetC(tid_t ios_tid);
int PutC(tid_t ios_tid, char c);

CIRCULAR_BUFFER_DEC(io_cb, char, 512);

#endif
