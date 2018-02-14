#include <kernel/handlers/msg.h>

void send_handler(TaskDescriptor *std) {
  // get the receiver td from the sender td
  int i, stid, r;
  asm("ldr %0, [%1, #4];":"=r"(stid):"r"(std->sp));
  stid = TID_ID(stid);
  KASSERT(IS_VALID_ID(stid));

  TaskDescriptor *rtd = &tasks[stid];

  KASSERT(rtd->status != TS_UNINIT);
  KASSERT(rtd->status != TS_ZOMBIE);
  if (rtd->status == TS_SND_BL) {
    // 1. copy message to receiver
    // 2. copy sender tid to receiver
    // 3. set receiver state to ready, re-add to queue
    // 4. set sender state to reply blocked

    int msg_len;
    tid_t *rtid;
    void *smsg, *rmsg;

    // load sender msg and msg_len
    asm("ldr %0, [%1, #8];":"=r"(smsg):"r"(std->sp));
    asm("ldr %0, [%1, #12];":"=r"(msg_len):"r"(std->sp));
    KASSERT(smsg != NULL);
    KASSERT(IS_VALID_USER_P(std->tid, smsg));
    KASSERT(msg_len > 0 && msg_len < 5000);

    // load receiver msg pointer
    asm("ldr %0, [%1, #4];":"=r"(rtid):"r"(rtd->sp));
    asm("ldr %0, [%1, #8];":"=r"(rmsg):"r"(rtd->sp));
    KASSERT(IS_VALID_USER_P(rtd->tid, rtid));
    KASSERT(IS_VALID_USER_P(rtd->tid, rmsg));
    KASSERT(rmsg != NULL);

    // 1.
    for (i = 0; i < msg_len; ++i) {
      *((char *)(rmsg+i)) = *((char *)(smsg+i));
    }

    // 2.
    *rtid = std->tid;

    // 3.
    rtd->status = TS_READY;
    pq_push(&pq_tasks, rtd->priority, rtd);

    // 4.
    std->status = TS_RPL_BL;
  } else {
    // 1. add to receiver send queue
    // 2. set sender state to receive blocked

    // 1.
    r = tid_cb_push(&(rtd->send_q), std->tid);
    KASSERT(r == 0);

    // 2.
    std->status = TS_RCV_BL;
  }
}

void receive_handler(TaskDescriptor *rtd) {
  // CircularBuffer *cb = &(rtd->send_q);
  tid_cb *cb = &(rtd->send_q);

  int i, r;
  tid_t stid;

  if (cb->size > 0) {
    // 1. copy message to receiver
    // 2. copy sender tid to receiver
    // 3. set receiver state to ready, re-add to queue
    // 4. set sender state to reply blocked

    int msg_len;
    tid_t *rtid;
    void *smsg, *rmsg;

    r = tid_cb_pop(cb, &stid);
    KASSERT(r == 0);

    stid = TID_ID(stid);
    KASSERT(IS_VALID_ID(stid));

    TaskDescriptor *std = &tasks[stid];

    KASSERT(std->status != TS_UNINIT && std->status != TS_ZOMBIE);

    // load sender msg and msg_len
    asm("ldr %0, [%1, #8];":"=r"(smsg):"r"(std->sp));
    asm("ldr %0, [%1, #12];":"=r"(msg_len):"r"(std->sp));
    KASSERT(smsg != NULL);
    KASSERT(IS_VALID_USER_P(std->tid, smsg));
    KASSERT(msg_len > 0 && msg_len < 5000);

    // load receiver msg pointer
    asm("ldr %0, [%1, #4];":"=r"(rtid):"r"(rtd->sp));
    asm("ldr %0, [%1, #8];":"=r"(rmsg):"r"(rtd->sp));
    KASSERT(IS_VALID_USER_P(rtd->tid, rtid));
    KASSERT(IS_VALID_USER_P(rtd->tid, rmsg));
    KASSERT(rmsg != NULL);

    // 1.
    for (i = 0; i < msg_len; ++i) {
      *((char *)(rmsg+i)) = *((char *)(smsg+i));
    }

    // 2.
    *rtid = std->tid;

    // 3.
    rtd->status = TS_READY;
    pq_push(&pq_tasks, rtd->priority, rtd);

    // 4.
    std->status = TS_RPL_BL;
  } else {
    rtd->status = TS_SND_BL;
  }
}


void reply_handler(TaskDescriptor *rtd) {
  // 1. reply is copied from receiver to sender
  // 2. sender's state is set to ready
  // 3. receiver's state is set to ready

  int i;
  tid_t stid;
  void *rreply, *sreply;
  int msg_len;

  // load the reply args
  asm("ldr %0, [%1, #4];":"=r"(stid):"r"(rtd->sp));
  asm("ldr %0, [%1, #8];":"=r"(rreply):"r"(rtd->sp));
  asm("ldr %0, [%1, #12];":"=r"(msg_len):"r"(rtd->sp));
  stid = TID_ID(stid);
  KASSERT(IS_VALID_ID(stid));
  KASSERT(IS_VALID_USER_P(rtd->tid, rreply));
  KASSERT(msg_len > 0 && msg_len < 5000);

  // load the reply arg from sender
  TaskDescriptor *std = &(tasks[stid]);
  asm("ldr %0, [%1, #16];":"=r"(sreply):"r"(std->sp));
  KASSERT(IS_VALID_USER_P(std->tid, sreply));

  // 1.
  for (i = 0; i < msg_len; ++i) {
    *((char *)(sreply + i)) = *((char *)(rreply + i));
  }

  // 2.
  std->status = TS_READY;
  pq_push(&pq_tasks, std->priority, std);

  // 3.
  rtd->status = TS_READY;
  pq_push(&pq_tasks, rtd->priority, rtd);
}
