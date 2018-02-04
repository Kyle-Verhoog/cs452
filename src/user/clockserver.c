#include <user/clockserver.h>


int Delay(int tid, uint32_t ticks) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(cs_tid > 0);

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_DELAY;
  req.ticks = ticks;

  int reply;
  Send(cs_tid, &req, &reply, sizeof(int));
  return reply;
}

int DelayUntil(int tid, int ticks) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(cs_tid > 0);

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_UNTIL;
  req.ticks = ticks;

  int reply;
  Send(cs_tid, &req, &reply, sizeof(int));
  return reply;
}

int Time(int tid) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(cs_tid > 0);

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_TIME;

  int reply;
  Send(cs_tid, &req, &reply, sizeof(int));
  return reply;
}

void ClockServer() {
  // init queue
  cs_queue csq;  
  csq_init(&csq);
  uint32_t ticks;
  ticks = 0;

  // init timer 3

  tid_t req_tid;
  CSReq req;

  int reply;

  while (true) {
    Receive(&req_tid, &req, sizeof(CSReq)); // TODO: AwaitEvent?

    switch (req.type) {
      case CSREQ_DELAY:
        if (req.ticks <= 0) {
          reply = -2
        } else {
          csq_add(&csq, TID_ID(req_tid), ticks + req.ticks);
          reply = 0;
        }
        Reply(req_tid, &reply, sizof(int));
        break;
      case CSREQ_UNTIL:
        if (req.ticks < ticks) {
          reply = -2;
        } else {
          csq_add(&csq, TID_ID(req_tid), req.ticks);
          reply = 0;
        }
        Reply(req_tid, &reply, sizof(int));
        break;
      case CSREQ_TIME:
        reply = ticks;
        Reply(req_tid, &reply, sizof(int));
        break;
      case CSREQ_UPDATE:
        ticks++;
        KASSERT(0 && "TODO");
        break;
      default:
        KASSERT(0 && "CS: UNKNOWN REQUEST TYPE");
        break;
    }
  }

  Exit();
}
