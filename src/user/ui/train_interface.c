#include <user/ui/train_interface.h>

static struct InterfaceTrain {
  bool exists;
  int tr_num;
  int speed;
  track_node *node;
};

static union InterfaceDatum {
  TETRSpeed    speed;
  TETRPosition pos;
  TETRStatus   status;
};

#define SPEED  1
#define POS    2
#define STATUS 3

static struct InterfaceData {
  int type;
  union InterfaceDatum event;
};

static void TrainSpeedSubscriber() {
  int r;
  tid_t rep_tid, par_tid;
  TrackRequest tr_req;
  InterfaceData data;

  data.type = SPEED;

  par_tid = MyParentTid();
  assert(par_tid > 0);
  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);

  tr_req.type = TRR_SUBSCRIBE;
  tr_req.data.type = TE_TR_SPEED;

  while (true) {
    Send(rep_tid, &tr_req, sizeof(tr_req), &data.event, sizeof(data.event));
    Send(par_tid, &data, sizeof(data), &r, sizeof(r));
  }
}

static void TrainPositionSubscriber() {
  int r;
  tid_t rep_tid, par_tid;
  TrackRequest tr_req;
  InterfaceData data;

  data.type = POS;

  par_tid = MyParentTid();
  assert(par_tid > 0);
  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);

  tr_req.type = TRR_SUBSCRIBE;
  tr_req.data.type = TE_TR_POS;

  while (true) {
    Send(rep_tid, &tr_req, sizeof(tr_req), &data.event, sizeof(data.event));
    Send(par_tid, &data, sizeof(data), &r, sizeof(r));
  }
}

void TrainInterface() {
  tid_t sub_tid, tm_tid;
  int i;
  struct InterfaceTrain trains[MAX_TRAIN];
  InterfaceData req;

  for (i = 0; i < MAX_TRAIN; ++i) {
    trains[i].num = i;
    trains[i].exists = false;
  }

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  TMRegister(tm_tid, TRAIN_OFF_X, TRAIN_OFF_Y, TRAIN_WIDTH, TRAIN_HEIGHT);

  Create(11, &TrainSpeedSubscriber);
  Create(11, &TrainPositionSubscriber);

  char c;
  while (true) {
    Receive(&sub_tid, &event, sizeof(event));
    switch (req.type) {
      case SPEED:
        trains[req.event.num].exists = true;
        trains[req.event.num].speed = req.event.speed;
        break;
      case POS:
        trains[req.event.num].exists = true;
        trains[req.event.num].pos = req.event.pos;
        break;
      case STATUS:
        trains[req.event.num].exists = true;
        trains[req.event.num].status = req.event.status;
        break;
      default: assert(0);
    }
    Reply(sub_tid, &i, sizeof(i));

    offset = 0;
    offset += buf_pack_c(buf+offset, TERM_RESET);
    offset += buf_pack_f(buf+offset, "   tr node    speed   \n");
    offset += buf_pack_f(buf+offset, "  ┉┉┉┉┉┉┉┉┉┉┉┉┉┉┉┉┉┉┉  \n");
    for (i = 0; i < MAX_TRAIN; ++i) {
      t = trains[i];
      if (t.exists) {
        offset += buf_pack_f(buf+offset, "   %d  %s    %d\n", t.num, t.pos->num, t.speed);
      }
    }
    TMPutStr(tm_tid, buf, offset);
  }

  Exit();
}
