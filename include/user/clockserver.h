#ifndef CLOCK_SERVER_H
#define CLOCK_SERVER_H

#include <stdlib.h>
#include <lib/clockserver_queue.h>
#include <kernel/syscalls.h>

#define CLOCKSERVER_ID 2

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


int Delay(int tid, uint32_t ticks);

int DelayUntil(int tid, int ticks);

int Time(int tid);

void ClockServer();

#endif /* CLOCK_SERVER_H */
