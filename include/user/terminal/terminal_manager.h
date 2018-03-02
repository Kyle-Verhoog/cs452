#ifndef TERMINAL_MANAGER_H
#define TERMINAL_MANAGER_H

#include <defines.h>
#include <user/syscalls.h>
#include <user/ioserver.h>
#include <user/terminal/terminal.h>

#define TERMINAL_UART_RX IOSERVER_UART2_RX_ID
#define TERMINAL_UART_TX IOSERVER_UART2_TX_ID

typedef enum TerminalManagerRequestType {
  TERM_IN  = 0,
  TERM_OUT = 1,
} TManReqType;

typedef struct TerminalManagerRequest {
  TManReqType type;
  char *data;
  int   len;
} TManReq;

typedef struct TerminalManagerReply {
  TManReqType type;
} TManRep;

void TerminalManager();
#endif
