#ifndef IOSERVER_H
#define IOSERVER_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>

typedef struct IONotifierArgs {
  int inter;
  int tid;
  int uart;
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


#endif
