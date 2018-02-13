#ifndef IOSERVER_H
#define IOSERVER_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>
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

/*
typedef struct IOServerNotifierRequest {
  int inttype;
} IOSNReq;
*/

void IOServerUART2();

CIRCULAR_BUFFER_DEC(io_cb, char, 512);

#endif
