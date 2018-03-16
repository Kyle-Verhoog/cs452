#include <user/train/m1_reset.h>

static tid_t tm_tid;

void init_train(tid_t tr_tid, int tr_num) {
  TMProtocol tm;
  int reply;
  tm.tmc = TM_MOVE;
  tm.arg1 = tr_num; //train
  tm.arg2 = 0; //train speed
  TMLogStrf(tm_tid, "  `tr %d 0`\n", tr_num);
  Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
}

void loop_train(tid_t tr_tid, int tr_num, int tr_spd) {
  TMProtocol tm;
  int reply;
  tm.tmc = TM_MOVE;
  tm.arg1 = tr_num; //train
  tm.arg2 = tr_spd; //train speed
  TMLogStrf(tm_tid, "  `tr %d %d`\n", tr_num, tr_spd);
  Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
}

void loop_switches(tid_t sw_tid, tid_t swpub_tid) {
  Switch *swlist;
  void *data;
  int reply, i;
  SWProtocol sw;
  sw.swr = SW_GET_ALL;
  Send(swpub_tid, &sw, sizeof(sw), &data, sizeof(data));
  swlist = (Switch *)data;

  for(i = NORMAL_SWITCH_SIZE_LOW; i <= NORMAL_SWITCH_SIZE_HIGH; ++i){
    if (swlist[i].state == SW_STRAIGHT) {
      sw.swr = SW_FLIP;
      sw.sw = i;
      sw.dir = SW_CURVE;
      Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
      TMLogStrf(tm_tid, "  `sw %d %c`\n", i, 'S');
    }
  }

  for(i = SPECIAL_SWITCH_SIZE_LOW; i <= SPECIAL_SWITCH_SIZE_HIGH; ++i){
    if (swlist[i].state != SW_STRAIGHT+(i%2)) {
      sw.dir = SW_STRAIGHT+(i%2);
      sw.sw = i;
      Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
      TMLogStrf(tm_tid, "  `sw %d %c`\n", i, (i%2) ? 'C' : 'S');
    }
  }

  sw.swr = SW_FLIP;
  sw.sw = 14;
  sw.dir = SW_STRAIGHT;
  Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
  TMLogStrf(tm_tid, "  `sw 14 S`\n");
  sw.sw = 17;
  sw.dir = SW_STRAIGHT;
  Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
  TMLogStrf(tm_tid, "  `sw 17 S`\n");
}

void M1Reset(void *arg) {
  int *args;
  int train_num, train_spd;
  tid_t tr_tid, sw_tid, swpub_tid, cs_tid;

  args = (int *)arg;

  train_num = args[0];
  train_spd = args[1];

  tr_tid = WhoIs(TRAIN_PROVIDER_ID);
  sw_tid = WhoIs(SWITCH_PROVIDER_ID);
  swpub_tid = WhoIs(SWITCH_PUBLISHER_ID);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(tr_tid > 0 && tm_tid > 0 && sw_tid > 0 && swpub_tid > 0 && cs_tid > 0);

  // TMRegister(tm_tid, M1_RESET_OFF_X, M1_RESET_OFF_Y, M1_RESET_WIDTH, M1_RESET_HEIGHT);

  TMLogStrf(tm_tid, "looping train %d at %d\n", train_num, train_spd);


  init_train(tr_tid, train_num);
  DelayCS(cs_tid, 200);
  loop_switches(sw_tid, swpub_tid);
  DelayCS(cs_tid, 300);
  loop_train(tr_tid, train_num, train_spd);
  DelayCS(cs_tid, 100);

  TMLogStrf(tm_tid, "reset complete!\n");

  // TMExit(tm_tid);
  Exit();
}
