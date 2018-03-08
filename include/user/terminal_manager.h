#ifndef TERMINAL_MANAGER_H
#define TERMINAL_MANAGER_H

#include <defines.h>
#include <lib/buffer_pack.h>
#include <conf/windows.h>
#include <user/syscalls.h>
#include <user/ioserver.h>
#include <lib/terminal.h>
#include <user/shell.h>

#define TERMINAL_UART_RX IOSERVER_UART2_RX_ID
#define TERMINAL_UART_TX IOSERVER_UART2_TX_ID

CIRCULAR_BUFFER_DEC(term_cb, char, TERMINAL_BUFFER_SIZE);

typedef enum TerminalManagerRequestType {
  TERM_IN      = 0,
  TERM_REG     = 1,
  TERM_OUT     = 2,
  TERM_GET     = 3,
  TERM_LOG_REG = 4,
  TERM_LOG     = 5,
  TERM_EXIT    = 6,
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
void TMPutStr(tid_t tm_tid, char *c, int len);
void TMPutStrf(tid_t tm_tid, char *fmt, ...);
char TMGetC(tid_t tm_tid);
void TMLogRegister(tid_t tm_tid);
void TMLogStrf(tid_t tm_tid, char *fmt, ...);
void TMExit(tid_t tm_tid);
#endif
