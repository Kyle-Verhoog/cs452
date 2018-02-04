#ifndef CLOCK_SERVER_H
#define CLOCK_SERVER_H

#include <stdlib.h>
#include <lib/clockserver_queue.h>
#include <ts7200.h>
#include <kernel/syscalls.h>
#include <user/nameserver.h>

#define CLOCKSERVER_ID 2

#define CS_SUCCESS  0
#define CS_E_TID   -1
#define CS_E_DELAY -2

#define CS_PROCESS_NUM 5

typedef enum CSReqType {
  CSREQ_DELAY  = 0,
  CSREQ_UNTIL  = 1,
  CSREQ_TIME   = 2,
  CSREQ_UPDATE = 3,
} CSReqType;

typedef struct CSReq {
  CSReqType type;
  tid_t     tid;
  uint32_t  ticks;
} CSReq;

typedef struct CSNReply {
  // cs_queue *csq;
  // uint32_t  ticks;
  tid_t *tids;
  int    ntids;
} CSNReply;


int Delay(int tid, uint32_t ticks);

int DelayUntil(int tid, int ticks);

int Time(int tid);

void ClockServer();

#endif /* CLOCK_SERVER_H */
