#include <user/train_commands.h>

void TrainTR(char *args) {
  assert(0);
  int reply, r;
  tid_t tr_tid, tm_tid, tid;
  TMProtocol tm;

  tm.tmc = TM_MOVE;

  tr_tid = WhoIs(TRAIN_MANAGER_ID);
  assert(tr_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  r = parse_args(args, "%d %d", &tm.arg1, &tm.arg2);
  if (r) {
    TMLogStrf(tm_tid, "tr: error parsing arg %d\n", r);
  }

  assert(tm.arg1 == 58);
  assert(tm.arg2 == 14);

  Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
  Receive(&tid, &r, sizeof(r));
  r = 1;
  Reply(tid, &r, sizeof(r));
  Exit();
}


void TrainRV(char *args) {
  int reply, r;
  tid_t tr_tid, tm_tid;
  TMProtocol tm;

  tm.tmc = TM_REVERSE;

  tr_tid = WhoIs(TRAIN_MANAGER_ID);
  assert(tr_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  r = parse_args(args, "%d", &tm.arg1);
  if (r) {
    TMLogStrf(tm_tid, "rv: error parsing arg %d\n", r);
  }

  Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
  Exit();
}
