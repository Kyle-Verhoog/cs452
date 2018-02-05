#include <user/clockserver.h>


int DelayCS(int tid, uint32_t ticks) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(IS_VALID_TID(cs_tid));
  return Delay(cs_tid, tid, ticks);
}

int Delay(int cs_tid, int tid, uint32_t ticks) {
  assert(IS_VALID_TID(tid));

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_DELAY;
  req.ticks = ticks;

  int reply;
  Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
  assert(reply == 0);
  return reply;
}

int DelayUntilCS(tid_t tid, uint32_t ticks) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(IS_VALID_TID(cs_tid));
  return DelayUntil(cs_tid, tid, ticks);
}

int DelayUntil(tid_t cs_tid, tid_t tid, uint32_t ticks) {
  assert(IS_VALID_TID(tid));

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_UNTIL;
  req.ticks = ticks;

  int reply;
  Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
  assert(reply == 0);
  return reply;
}


int TimeCS(tid_t tid) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(IS_VALID_TID(cs_tid));
  return Time(cs_tid, tid);
}

int Time(tid_t cs_tid, tid_t tid) {
  assert(IS_VALID_TID(tid));

  CSReq req;
  req.tid   = tid;
  req.type  = CSREQ_TIME;

  int reply;
  Send(cs_tid, &req, sizeof(req), &reply, sizeof(int));
  assert(reply >= 0);
  return reply;
}

void ClockServerStop() {
  CSReq req;
  tid_t cs_tid;
  int reply;

  req.type = CSREQ_HALT;
  cs_tid = WhoIs(CLOCKSERVER_ID);

  Send(cs_tid, &req, sizeof(req), &reply, sizeof(reply));
  Exit();
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
  assert(cs_tid > 0);

  while (true) {
    AwaitEvent(IE_TC3UI);
    Send(cs_tid, &req, sizeof(req), &reply, sizeof(reply));
    // PRINTF("%d\n\r", *(unsigned int*)(TIMER3_BASE | VAL_OFFSET));
    assert(reply.ntids >= -1 && reply.ntids <= CS_PROCESS_NUM);
    assert(reply.tids != NULL);
    if (reply.ntids == CSN_EXIT_CODE) {
      Exit();
    }

    for (i = 0; i < reply.ntids; i++) {
      tid_t tid = reply.tids[i];
      assert(IS_VALID_TID(tid));
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
  tid_t req_tid, id, not_tid;
  CSReq req;
  int i, ret, reply, exit_flag;
  CSNReply nreply;
  uint32_t ticks;

  ret = RegisterAs(CLOCKSERVER_ID);

  // init queue, ticks and reply
  csq_init(&csq);
  ticks = 0;
  nreply.tids = reply_tids;
  exit_flag = 0;

  not_tid = Create(31, &ClockServerNotifier);
  assert(IS_VALID_TID(not_tid));

  // init timer 3
  *(int*)(CS_TIMER_LOAD) = CS_TIMER_VALUE;
  *(int*)(CS_TIMER_CTRL) = CS_TIMER_FLAGS;

  while (true) {
    Receive(&req_tid, &req, sizeof(CSReq));
    assert(IS_VALID_TID(req_tid));

    switch (req.type) {
      case CSREQ_DELAY:
        assert(req.ticks > 0);
        assert(IS_VALID_TID(req.tid));
        if (req.ticks <= 0) {
          assert(0);
          reply = CS_E_DELAY;
          Reply(req_tid, &reply, sizeof(int));
          break;
        }
        ret = csq_add(&csq, req_tid, ticks + req.ticks);
        assert(ret == 0);
        break;
      case CSREQ_UNTIL:
        assert(req.ticks > 0);
        assert(IS_VALID_TID(req.tid));
        if (req.ticks < ticks) {
          assert(0);
          reply = CS_E_DELAY;
          Reply(req_tid, &reply, sizeof(int));
          break;
        }
        ret = csq_add(&csq, req_tid, req.ticks);
        assert(ret == 0);
        break;
      case CSREQ_TIME:
        reply = ticks;
        Reply(req_tid, &reply, sizeof(int));
        break;
      case CSREQ_UPDATE:
        if (exit_flag) {
          nreply.ntids = CSN_EXIT_CODE;
          Reply(not_tid, &nreply, sizeof(nreply));
          Exit();
        }

        ticks++;
        // PRINTF("%d\r\n", ticks);

        // load the ready tasks into the transfer array
        nreply.ntids = 0;
        for (i = 0; i < CS_PROCESS_NUM; i++) {
          if (csq_pop(&csq, ticks, &id) != 0) {
            break;
          }
          reply_tids[i] = id;
          nreply.ntids++;
        }

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
      case CSREQ_HALT:
        // set flag to exit on next request
        exit_flag = 1;

        // reply to sender of halt
        Reply(req_tid, &reply, sizeof(reply));
        break;
      default:
        assert(0 && "CS: UNKNOWN REQUEST TYPE");
        break;
    }
  }

  Exit();
}
