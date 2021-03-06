#ifndef IOSERVER_H
#define IOSERVER_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>
#include <user/clockserver.h>
#include <user/uio.h>
#include <debug/debug.h>
#include <lib/ioserver_buffer.h>


typedef struct IOServerArgs {
  InterruptEvent ie_base;
  int uart_base;
  int ns_id;
  int flags;
  bool cts_en;
} IOServerArgs;

typedef enum IOServerRequestType {
  IO_GETC   = 0,
  IO_PUTC   = 1,
  IO_TX     = 2,
  IO_RX     = 3,
  IO_RT     = 4,
  IO_MI     = 5,
  IO_PUTSTR = 6,
  IO_BLPUTC = 7,
  IO_FLUSH = 8,
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
  int  len;
  IOServerReqType type; // Note: the type must be the last (first) element!
} IOServerReq;



void IOServerUART2();
void IOServerUART1();

char GetC(tid_t ios_tid);
int PutC(tid_t ios_tid, char c);
int BLPutC(tid_t ios_tid, char c);
int PutStr(tid_t ios_tid, char *c, int len);
int FlushIO(tid_t ios_tid);


#endif
