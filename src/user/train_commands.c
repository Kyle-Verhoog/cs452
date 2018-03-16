#include <user/train_commands.h>

void TrainTR(char *args) {
  int r, tn, ts;
  tid_t tr_tid, tm_tid, tid;
  TMProtocol tm;

  tm.tmc = TM_MOVE;

  tr_tid = WhoIs(TRAIN_MANAGER_ID);
  assert(tr_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %d", &tn, &ts);

  if (r) {
    TMLogStrf(tm_tid, "tr: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  tm.arg1 = (char)tn;
  tm.arg2 = (char)ts;
  TMLogStrf(tm_tid, "tr %d %d\n", tm.arg1, tm.arg2);

  Send(tr_tid, &tm, sizeof(tm), &r, sizeof(r));
  r = 0;
  Reply(tid, &r, sizeof(r));
  Exit();
}


void TrainRV(char *args) {
  int tn, r;
  tid_t tr_tid, tm_tid, tid;
  TMProtocol tm;

  tm.tmc = TM_REVERSE;

  tr_tid = WhoIs(TRAIN_MANAGER_ID);
  assert(tr_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d", &tn);
  if (r) {
    TMLogStrf(tm_tid, "rv: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  tm.arg1 = (char)tn;

  Send(tr_tid, &tm, sizeof(tm), &r, sizeof(r));
  Exit();
}


void TrainTK(char *args) {
  int tn, s, r;
  char stn1[5];
  TMProtocol tm;
  tid_t tr_tid, tm_tid, tid;
  tm.tmc = TM_TRACK;

  // tr_tid = WhoIs(TRAIN_MANAGER_ID);
  // assert(tr_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %s", &tn, stn1, sizeof(stn1));
  if (r) {
    TMLogStrf(tm_tid, "tk: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  s = trhr(track, stn1);
  if (s < 0 || s > TRACK_MAX-1) {
    TMLogStrf(tm_tid, "tk: invalid track node %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  tm.arg1 = (char)tn;
  tm.arg2 = (char)s;
  TMLogStrf(tm_tid, "tk: %d %d\n", tn, s);
  // Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
  r = 0;
  Reply(tid, &r, sizeof(r));
  Exit();
}
