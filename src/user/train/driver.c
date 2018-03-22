#include <user/train/driver.h>

#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/terminal_manager.h>
#include <lib/train/train_defines.h>
#include <user/train/representer.h>
#include <user/train/reservation_manager.h>


static tid_t tm_tid, sw_tid, rm_tid;

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


#define LOOK_AHEAD 1000
void HandlePositionUpdate(TDTrain *train, track_node *new_pos) {
  track_node *tn;
  SWProtocol sw;
  sw_configs sw_cfgs;
  sw_config cfg;

  sw.swr = SW_FLIP;
  sw_configs_init(&sw_cfgs);

  int r;
  if (!train->pos) {
    path_start(&train->p, train->p.start);
  }

  r = path_follow_to(&train->p, new_pos);

  if (r < 0) {
    TMLogStrf(tm_tid, "PATH LOST :*(\n");

    // free all the trains nodes
    // r = Free(rm_tid, train->num, train->pos);
    // if (r) TMLogStrf(tm_tid, "FREE FAILED\n");

    tn = train->p.end;
    path_init(&train->p, TRACK);
    path_set_destination(&train->p, new_pos, tn);
    path_generate(&train->p);
    path_start(&train->p, train->p.start);
  }


  // TODO: get stopping distance
  train->last_last_pos = train->last_pos;
  train->last_pos = train->pos;
  train->pos = new_pos;
  r = Reserve(rm_tid, train->num, train->pos, 500);
  if (r) TMLogStrf(tm_tid, "RESERVE ERR %d", r);

  if (train->last_last_pos) {
    r = Free(rm_tid, train->num, train->last_last_pos);
    if (r) TMLogStrf(tm_tid, "FREE ERR %d", r);
  }

  path_switches_in_next_dist(&train->p, &sw_cfgs, LOOK_AHEAD);
  while (sw_cfgs.size > 0) {
    sw_configs_pop(&sw_cfgs, &cfg);
    sw.sw  = cfg.sw->num;
    sw.dir = cfg.state_required == DIR_CURVED ? SW_CURVE : SW_STRAIGHT;
    Send(sw_tid, &sw, sizeof(sw), &r, sizeof(r));
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

  rm_tid = WhoIs(RESERVATION_MANAGER_ID);
  assert(rm_tid > 0);

  TMRegister(tm_tid, DRIVER1_OFF_X, DRIVER1_OFF_Y, DRIVER1_WIDTH, DRIVER1_HEIGHT);

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
