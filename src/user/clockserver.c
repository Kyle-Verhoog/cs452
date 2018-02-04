#include <user/clockserver.h>


int Delay(int tid, uint32_t ticks) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(IS_VALID_TID(cs_tid));

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_DELAY;
  req.ticks = ticks;

  int reply;
  Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
  KASSERT(reply == 0);
  return reply;
}

int DelayUntil(int tid, int ticks) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  KASSERT(IS_VALID_TID(cs_tid));

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_UNTIL;
  req.ticks = ticks;

  int reply;
  Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
  KASSERT(reply == 0);
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
  KASSERT(reply >= 0);
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
 * Keeps track of 10ms ticks once started and provides functionality for
 * other tasks to block themselves on a specific tick.
 *
 * Note: must be notified of hwclock ticks via ClockServerNotify()
 */
void ClockServer() {
  // init queue
  cs_queue csq;  
  tid_t reply_tids[CS_PROCESS_NUM];
  tid_t req_tid, id;
  CSReq req;
  int i, ret, reply;
  CSNReply nreply;
  uint32_t ticks;

  // init queue, ticks and reply
  csq_init(&csq);
  ticks = 0;
  nreply.tids = reply_tids;

  // init timer 3
  *(int*)(CS_TIMER_LOAD) = CS_TIMER_VALUE;
  *(int*)(CS_TIMER_CTRL) = CS_TIMER_FLAGS;

  while (true) {
    Receive(&req_tid, &req, sizeof(CSReq));
    KASSERT(IS_VALID_TID(req_tid));

    switch (req.type) {
      case CSREQ_DELAY:
        KASSERT(req.ticks > 0);
        KASSERT(IS_VALID_TID(req.tid));
        if (req.ticks <= 0) {
          KASSERT(0);
          reply = CS_E_DELAY;
          Reply(req_tid, &reply, sizeof(int));
          break;
        }
        ret = csq_add(&csq, req_tid, ticks + req.ticks);
        KASSERT(ret == 0);
        break;
      case CSREQ_UNTIL:
        KASSERT(req.ticks > 0);
        KASSERT(IS_VALID_TID(req.tid));
        if (req.ticks < ticks) {
          KASSERT(0);
          reply = CS_E_DELAY;
          Reply(req_tid, &reply, sizeof(int));
          break;
        }
        ret = csq_add(&csq, req_tid, req.ticks);
        KASSERT(ret == 0);
        break;
      case CSREQ_TIME:
        reply = ticks;
        Reply(req_tid, &reply, sizeof(int));
        break;
      case CSREQ_UPDATE:
        ticks++;
        nreply.ntids = 0;

        // load the ready tasks into the transfer array
        for (i = 0; i < CS_PROCESS_NUM; i++) {
          if (csq_pop(&csq, ticks, &id) == 0) {
            break;
          }
          reply_tids[i] = id;
          nreply.ntids++;
        }

        // nreply.ticks = ticks;
        // nreply.csq   = &csq;

        // send ready tasks to notifier to deal with
        // TODO?: we send the tasks back to the notifier which also handles
        //        the hw interrupt.
        //        if the notifier gets hw interrupted while processing the
        //        ready tasks we're screwed
        //        the counter to this is that since we just got here from
        //        the notifier, we have ~10ms to get the ready tasks to the
        //        notifier and dealt with before the next interrupt.
        Reply(req_tid, &nreply, sizeof(nreply));
        break;
      default:
        KASSERT(0 && "CS: UNKNOWN REQUEST TYPE");
        break;
    }
  }

  Exit();
}
