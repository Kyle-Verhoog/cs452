#include <user/train/driver.h>

#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/terminal_manager.h>
#include <lib/train/train_defines.h>
#include <user/train/representer.h>
#include <user/train/reservation_manager.h>

bool DRIVER1_DEF, DRIVER2_DEF;

static tid_t tm_tid, sw_tid, rm_tid, tr_tid;

typedef struct TDTrain {
  int num;
  int speed;
  track_node *pos;
  track_node *last_pos;
  track_node *last_last_pos;
  path p;
} TDTrain;

void TDTrainInit(TDTrain *train, int num) {
  train->num = num;
  train->speed = -1;
  train->pos = NULL;
  train->last_pos = NULL;
  train->last_last_pos = NULL;
  path_init(&train->p, TRACK);
}

union TDDatum {
  TETRSpeed    speed;
  TETRPosition pos;
};

#define SPEED  1
#define POS    2
#define STATUS 3

struct TDData {
  int type;
  union TDDatum event;
};


static void TrainPositionSubscriber() {
  int r;
  tid_t rep_tid, par_tid;
  TrackRequest tr_req;
  struct TDData data;

  data.type = POS;

  par_tid = MyParentTid();
  assert(par_tid > 0);
  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);

  tr_req.type = TRR_SUBSCRIBE;
  tr_req.data.type = TE_TR_POSITION;

  while (true) {
    Send(rep_tid, &tr_req, sizeof(tr_req), &data.event, sizeof(data.event));
    Send(par_tid, &data, sizeof(data), &r, sizeof(r));
  }
}


static void FlipSwitch(sw_config *cfg) {
  SWProtocol sw;
  int r;
  sw.swr = SW_FLIP;
  sw.sw  = cfg->sw->num;
  sw.dir = cfg->state_required == DIR_CURVED ? SW_CURVE : SW_STRAIGHT;
  Send(sw_tid, &sw, sizeof(sw), &r, sizeof(r));
}

static void TrainCmd(int tr_num, int cmd) {
  int r;
  TrainProtocol tp;
  tp.tc = T_MOVE;
  tp.arg1 = (char)tr_num;
  tp.arg2 = (char)cmd;
  Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));
}


#define LOOK_AHEAD 1000
static void HandlePositionUpdate(TDTrain *train, track_node *new_pos) {
  track_node *tn;
  sw_configs sw_cfgs;
  sw_config cfg;

  sw_configs_init(&sw_cfgs);

  int r;
  if (!train->pos) {
    path_start(&train->p, train->p.start);
  }

  r = path_follow_to(&train->p, new_pos);

  if (r < 0) {
    TMLogStrf(tm_tid, "PATH LOST: recalculating\n");

    // free all the trains nodes
    if (train->pos) {
      r = Free(rm_tid, train->num, train->pos);
      if (r) TMLogStrf(tm_tid, "FREE ALL FAILED\n");
    }

    tn = train->p.end;
    path_init(&train->p, TRACK);
    path_set_destination(&train->p, new_pos, tn);
    path_generate(&train->p);
    // r = PathFind(rm_tid, train->num, &train->p);
    if (r) TMLogStrf(tm_tid, "RECALCULATING FAILED :/\n");
    path_start(&train->p, train->p.start);
  }

  train->last_last_pos = train->last_pos;
  train->last_pos = train->pos;
  train->pos = new_pos;
  // TODO: get stopping distance
  r = Reserve(rm_tid, train->num, train->pos, 500);
  if (r) {
    TMLogStrf(tm_tid, "RESERVATION CONFLICT\n");
    TrainCmd(train->num, 0);
  }

  if (train->last_last_pos) {
    r = Free(rm_tid, train->num, train->last_last_pos);
    if (r) TMLogStrf(tm_tid, "FREE ERR %d\n", r);
  }

  path_switches_in_next_dist(&train->p, &sw_cfgs, LOOK_AHEAD);
  while (sw_cfgs.size > 0) {
    sw_configs_pop(&sw_cfgs, &cfg);
    FlipSwitch(&cfg);
  }
}

void TrainDriver(TrainDriverArgs *args) {
  int r;
  tid_t sub_tid;
  struct TDData req;
  TDTrain train;
  char buf[4096];

  TDTrainInit(&train, args->train_num);

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  tr_tid = WhoIs(TRAIN_PROVIDER_ID);
  assert(tr_tid > 0);

  rm_tid = WhoIs(RESERVATION_MANAGER_ID);
  assert(rm_tid > 0);

  if (!DRIVER1_DEF) {
    DRIVER1_DEF = true;
    TMRegister(tm_tid, DRIVER1_OFF_X, DRIVER1_OFF_Y, DRIVER1_WIDTH, DRIVER1_HEIGHT);
  }
  else if (!DRIVER2_DEF) {
    DRIVER2_DEF = true;
    TMRegister(tm_tid, DRIVER2_OFF_X, DRIVER2_OFF_Y, DRIVER2_WIDTH, DRIVER2_HEIGHT);
  }

  path_set_destination(&train.p, &TRACK[args->start], &TRACK[args->end]);

  r = path_generate(&train.p);
  if (r < 0) {
    TMLogStrf(tm_tid, "COULD NOT CALCULATE PATH\n");
    Exit();
  }

  sw_tid = WhoIs(SWITCH_PROVIDER_ID);
  assert(sw_tid > 0);

  Create(21, &TrainPositionSubscriber);

  while (true) {
    Receive(&sub_tid, &req, sizeof(req));

    switch (req.type) {
      case POS:
        HandlePositionUpdate(&train, req.event.pos.node);
        break;
      default: assert(0);
    }

    path_to_str(&train.p, buf);
    TMPutStrf(tm_tid, "\r%d: %s", train.num, buf);
    Reply(sub_tid, &r, sizeof(r));
  }
}
