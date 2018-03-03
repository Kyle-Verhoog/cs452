#ifndef TERMINAL_MANAGER_H
#define TERMINAL_MANAGER_H

#include <defines.h>
#include <user/syscalls.h>
#include <user/ioserver.h>
#include <user/terminal/terminal.h>
#include <user/shell.h>

#define TERMINAL_UART_RX IOSERVER_UART2_RX_ID
#define TERMINAL_UART_TX IOSERVER_UART2_TX_ID

CIRCULAR_BUFFER_DEC(term_cb, char, TERMINAL_BUFFER_SIZE);

typedef enum TerminalManagerRequestType {
  TERM_IN  = 0,
  TERM_REG = 1,
  TERM_OUT = 2,
  TERM_GET = 3,
} TManReqType;

typedef struct TerminalManagerRequest {
  TManReqType type;
  char *data;
  int   len;
} TManReq;

typedef struct TerminalManagerReply {
  TManReqType type;
  char data;
} TManRep;

typedef struct WindowManager {
  TDisplay td;
  int x;
  int y;
} WManager;

void TerminalManager();
void TMRegister(tid_t tm_tid, char offsetx, char offsety, char width, char height);
void TMPutC(tid_t tm_tid, char c);
char TMGetC(tid_t tm_tid);
#endif
