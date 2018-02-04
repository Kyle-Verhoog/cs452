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
  int i, ureply, cs_tid;
  CSReq req;
  CSNReply reply;

  req.type = CSREQ_UPDATE;
  cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(cs_tid > 0);

  while (true) {
    // TODO: AwaitEvent(TIMER_3);
    Send(cs_tid, &req, sizeof(req), &reply, sizeof(reply));
    KASSERT(reply.ntids >= 0 && reply.ntids <= CS_PROCESS_NUM);
    KASSERT(reply.tids != NULL);
    for (i = 0; i < reply.ntids; i++) {
      tid_t tid = reply.tids[i];
      KASSERT(IS_VALID_TID(tid));
      ureply = 0;
      Reply(tid, &ureply, sizeof(ureply));
    }
  }
}


/**
 * Keeps track of 10ms ticks once started.
 *
 * Note: must be notified of hwclock ticks via ClockServerNotify()
 */
void ClockServer() {
  // init queue
  cs_queue csq;  
  tid_t reply_tids[CS_PROCESS_NUM];
  tid_t req_tid, id;
  CSReq req;
  int i, reply;
  CSNReply nreply;
  uint32_t ticks;

  csq_init(&csq);
  ticks = 0;
  nreply.tids  = reply_tids;

  // init timer 3
  *(int*)(TIMER3_BASE | LDR_OFFSET) = 5020;
  *(int*)(TIMER3_BASE | CRTL_OFFSET) = ENABLE_MASK | CLKSEL_MASK | MODE_MASK;

  while (true) {
    Receive(&req_tid, &req, sizeof(CSReq));

    switch (req.type) {
      case CSREQ_DELAY:
        if (req.ticks <= 0) {
          reply = CS_E_DELAY;
          Reply(req_tid, &reply, sizeof(int));
          break;
        }
        csq_add(&csq, req_tid, ticks + req.ticks);
        break;
      case CSREQ_UNTIL:
        if (req.ticks < ticks) {
          reply = CS_E_DELAY;
          Reply(req_tid, &reply, sizeof(int));
          break;
        }
        csq_add(&csq, req_tid, req.ticks);
        break;
      case CSREQ_TIME:
        reply = ticks;
        Reply(req_tid, &reply, sizeof(int));
        break;
      case CSREQ_UPDATE:
        ticks++;
        nreply.ntids = 0;
        for (i = 0; i < CS_PROCESS_NUM; i++) {
          if (csq_pop(&csq, ticks, &id) == 0) {
            break;
          }
          reply_tids[i] = id;
          nreply.ntids++;
        }

        // nreply.ticks = ticks;
        // nreply.csq   = &csq;
        Reply(req_tid, &nreply, sizeof(nreply));
        break;
      default:
        KASSERT(0 && "CS: UNKNOWN REQUEST TYPE");
        break;
    }
  }

  Exit();
}
