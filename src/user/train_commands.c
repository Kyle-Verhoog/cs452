#include <user/train_commands.h>

void TrainTR(char *args) {
  int r, tn, ts;
  tid_t tr_tid, tm_tid, tid;
  TrainProtocol tp;

  tp.tc = T_MOVE;

  tr_tid = WhoIs(TRAIN_PROVIDER_ID);
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

  tp.arg1 = (char)tn;
  tp.arg2 = (char)ts;
  TMLogStrf(tm_tid, "tr %d %d\n", tp.arg1, tp.arg2);

  Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));
  r = 0;
  Reply(tid, &r, sizeof(r));
  Exit();
}


// void TrainRV(char *args) {
//   int tn, r;
//   tid_t tr_tid, tm_tid, tid;
//   TMProtocol tm;

//   tm.tmc = TM_REVERSE;

//   tr_tid = WhoIs(TRAIN_PROVIDER_ID);
//   assert(tr_tid > 0);
//   tm_tid = WhoIs(TERMINAL_MANAGER_ID);
//   assert(tm_tid > 0);

//   Receive(&tid, &r, sizeof(r));
//   r = parse_args(args, "%d", &tn);
//   if (r) {
//     TMLogStrf(tm_tid, "rv: error parsing arg %d\n", r);
//     r = -1;
//     Reply(tid, &r, sizeof(r));
//     Exit();
//   }

//   tm.arg1 = (char)tn;

//   Send(tr_tid, &tm, sizeof(tm), &r, sizeof(r));
//   r = 0;
//   Reply(tid, &r, sizeof(r));
//   Exit();
// }

void SwitchSW(char *args){
  int swn, r;
  char dir;
  tid_t sw_tid, tm_tid, tid;
  SWProtocol sw;

  sw_tid = WhoIs(SWITCH_PROVIDER_ID);
  assert(sw_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));

  sw.swr = SW_FLIP;
  r = parse_args(args, "%d %c", &swn, &dir);
  if (r) {
    TMLogStrf(tm_tid, "rv: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }
  if (dir != 'C' && dir != 'S') {
    TMLogStrf(tm_tid, "rv: invalid sw dir!\n");
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  if (!((NORMAL_SWITCH_SIZE_LOW <= swn && swn <= NORMAL_SWITCH_SIZE_HIGH) ||
        (SPECIAL_SWITCH_SIZE_LOW <= swn && swn <= SPECIAL_SWITCH_SIZE_HIGH))) {
    TMLogStrf(tm_tid, "rv: invalid sw num!\n");
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  sw.sw = swn;
  sw.dir = dir == 'C' ? SW_CURVE : SW_STRAIGHT;

  Send(sw_tid, &sw, sizeof(sw), &r, sizeof(r));
  r = 0;
  Reply(tid, &r, sizeof(r));
  Exit();
}

//TODO:TK now is started from prediction manager
// void TrainTK(char *args) {
//   int tn, s, r;
//   char stn1[5];
//   TMProtocol tm;
//   tid_t tr_tid, tm_tid, tid;
//   tm.tmc = TM_TRACK;

//   // tr_tid = WhoIs(TRAIN_PROVIDER_ID);
//   // assert(tr_tid > 0);
//   tm_tid = WhoIs(TERMINAL_MANAGER_ID);
//   assert(tm_tid > 0);

//   Receive(&tid, &r, sizeof(r));
//   r = parse_args(args, "%d %s", &tn, stn1, sizeof(stn1));
//   if (r) {
//     TMLogStrf(tm_tid, "tk: error parsing arg %d\n", r);
//     r = -1;
//     Reply(tid, &r, sizeof(r));
//     Exit();
//   }

//   s = trhr(track, stn1);
//   if (s < 0 || s > TRACK_MAX-1) {
//     TMLogStrf(tm_tid, "tk: invalid track node %d\n", r);
//     r = -1;
//     Reply(tid, &r, sizeof(r));
//     Exit();
//   }

//   tm.arg1 = (char)tn;
//   tm.arg2 = (char)s;
//   TMLogStrf(tm_tid, "tk: %d %d\n", tn, s);
//   // Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
//   r = 0;
//   Reply(tid, &r, sizeof(r));
//   Exit();
// }
