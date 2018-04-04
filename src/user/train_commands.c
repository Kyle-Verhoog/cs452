#include <user/train_commands.h>

void TrainDR(char *args) {
  int tn, s1, s2, r;
  char stn1[10], stn2[10];
  TrainDriverArgs tdargs;
  tid_t tm_tid, tid;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %s %s", &tn, stn1, sizeof(stn1), stn2, sizeof(stn2));
  if (r) {
    TMLogStrf(tm_tid, "dr: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  s1 = trhr(TRACK, stn1);
  s2 = trhr(TRACK, stn2);
  if (s1 < 0 || s1 > TRACK_MAX-1) {
    TMLogStrf(tm_tid, "dr: invalid track node %d\n", 2);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }
  if (s2 < 0 || s2 > TRACK_MAX-1) {
    TMLogStrf(tm_tid, "dr: invalid track node %d\n", 3);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  tdargs.train_num = tn;
  tdargs.start = s1;
  tdargs.end = s2;
  TMLogStrf(tm_tid, "dr: %d %d %d\n", tn, s1, s2);
  CreateArgs(PRI_TRAIN_DRIVER, &TrainDriver, &tdargs, sizeof(tdargs));
  r = 0;
  Reply(tid, &r, sizeof(r));
  Exit();
}


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

  if (!((NORM_SW_LOW <= swn && swn <= NORM_SW_HIGH) ||
        (SPEC_SW_LOW <= swn && swn <= SPEC_SW_HIGH))) {
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

void TrainTK(char *args){
  int r, tn, p;
  char pos[10];
  tid_t tr_tid, tm_tid, tid;
  TrainProtocol tp;

  tp.tc = T_INIT;

  tr_tid = WhoIs(TRAIN_PROVIDER_ID);
  assert(tr_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %s", &tn, pos, sizeof(pos));

  if (r) {
    TMLogStrf(tm_tid, "tk: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  p = trhr(TRACK, pos);

  if(p < 0 || p > TRACK_MAX-1){
    TMLogStrf(tm_tid, "tk: invalid track node %d\n", p);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  tp.arg1 = (char)tn;
  tp.arg2 = (char)p;
  TMLogStrf(tm_tid, "tk %d %d\n", tp.arg1, tp.arg2);

  Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));
  r = 0;
  Reply(tid, &r, sizeof(r));
  Exit(); 
}

void TrainCAL(char *args) {
  CalibrationArgs cargs;
  int r;
  tid_t tm_tid, tid;
  char node[10];

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %d %s", &cargs.train, &cargs.init_gear, node, sizeof(node));
  if (r) {
    TMLogStrf(tm_tid, "cal: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  cargs.target_node = trhr(TRACK, node);
  if(cargs.target_node < 0 || cargs.target_node > TRACK_MAX-1){
    TMLogStrf(tm_tid, "cal: invalid track node %d\n", cargs.target_node);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }
  
  CreateArgs(19, &Calibration, (void *)&cargs, sizeof(cargs));

  Reply(tid, &r, sizeof(r));
  Exit();
}

void TrainACAL(char *args){
  CalibrationArgs cargs;
  int r;
  tid_t tm_tid, tid;
  char node[10];

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %d %s", &cargs.train, &cargs.init_gear, node, sizeof(node));
  if (r) {
    TMLogStrf(tm_tid, "acal: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  cargs.target_node = trhr(TRACK, node);
  if(cargs.target_node < 0 || cargs.target_node > TRACK_MAX-1){
    TMLogStrf(tm_tid, "acal: invalid track node %d\n", cargs.target_node);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }
  
  CreateArgs(19, &AccelCalibration, (void *)&cargs, sizeof(cargs));

  Reply(tid, &r, sizeof(r));
  Exit(); 
}

void TrainMS(char *args) {
  int train, r;
  tid_t tm_tid, tid;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d", &train);
  if (r) {
    TMLogStrf(tm_tid, "ms: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }
  
  CreateArgs(19, &MeasuringVelocity, (void *)&train, sizeof(train));

  Reply(tid, &r, sizeof(r));
  Exit();
}

void SetTrainMS(char *args) {
  int ms, r;
  tid_t tm_tid, tid;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d", &ms);
  if (r) {
    TMLogStrf(tm_tid, "sms: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }
  
  CreateArgs(19, &SetMS, (void *)&ms, sizeof(ms));

  Reply(tid, &r, sizeof(r));
  Exit();
}


void TrainTST(char *args){
  int r;
  TestCalibArgs tcargs;
  tid_t tm_tid, tid;
  char node[10];

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %d %d %s", &tcargs.train, &tcargs.gear, &tcargs.dist, node, sizeof(node));
  if (r) {
    TMLogStrf(tm_tid, "ms: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  tcargs.target_node = trhr(TRACK, node);
  
  if(tcargs.target_node < 0 || tcargs.target_node > TRACK_MAX-1){
    TMLogStrf(tm_tid, "tst: invalid track node %d\n", tcargs.target_node);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  CreateArgs(19, &TestCalibration, (void *)&tcargs, sizeof(tcargs));

  Reply(tid, &r, sizeof(r));
  Exit();
}

void TrainATST(char *args){
  int r;
  ATestCalibArgs tcargs;
  tid_t tm_tid, tid;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %d %d %d", &tcargs.train, &tcargs.gear, &tcargs.delay, &tcargs.dist);
  if (r) {
    TMLogStrf(tm_tid, "ms: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }
  
  CreateArgs(19, &ATestCalibration, (void *)&tcargs, sizeof(tcargs));

  Reply(tid, &r, sizeof(r));
  Exit();
}
