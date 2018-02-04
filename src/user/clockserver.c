#include <user/clockserver.h>


int Delay(int tid, uint32_t ticks) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(cs_tid > 0);

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_DELAY;
  req.ticks = ticks;

  int reply;
  Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
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
  Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
  return reply;
}

int Time(int tid) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(cs_tid > 0);

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_TIME;

  int reply;
  Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
  return reply;
}

/**
 * Notifies the clock server that a tick has occurred.
 * Note: this is a notifier, and should be given max priority.
 */
void ClockServerNotifier() {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(cs_tid > 0);

  CSReq req;
  req.type = CSREQ_UPDATE;

  int reply;
  return reply;

  CSReq req;
  int reply;

  while (true) {
    // TODO: AwaitEvent(TIMER_3);
    Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
    KASSERT(reply == 0);
  }
}

/**
 * Keeps track of 10ms ticks once started.
 *
 * Note: must be notified via ClockServerNotify()
 */
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
    Receive(&req_tid, &req, sizeof(CSReq));

    switch (req.type) {
      case CSREQ_DELAY:
        if (req.ticks <= 0) {
          reply = CS_E_DELAY;
        } else {
          csq_add(&csq, TID_ID(req_tid), ticks + req.ticks);
          reply = CS_SUCCESS;
        }
        Reply(req_tid, &reply, sizeof(int));
        break;
      case CSREQ_UNTIL:
        if (req.ticks < ticks) {
          reply = CS_E_DELAY;
        } else {
          csq_add(&csq, TID_ID(req_tid), req.ticks);
          reply = CS_SUCCESS;
        }
        Reply(req_tid, &reply, sizeof(int));
        break;
      case CSREQ_TIME:
        reply = ticks;
        Reply(req_tid, &reply, sizeof(int));
        break;
      case CSREQ_UPDATE:
        ticks++;
        reply = 0;
        Reply(req_tid, &reply, sizeof(int));
        break;
      default:
        KASSERT(0 && "CS: UNKNOWN REQUEST TYPE");
        break;
    }
  }

  Exit();
}
