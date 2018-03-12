#include <user/train_commands.h>

void TrainTR(void *args) {
  tid_t tr_tid;
  int reply;

  tr_tid = WhoIs(TRAIN_MANAGER_ID);
  assert(tr_tid > 0);
  TMProtocol tm;
  tm.tmc = TM_MOVE;
  tm.arg1 = *(int *)args++; //train
  tm.arg2 = *(int *)args;   //train speed
  Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
}
