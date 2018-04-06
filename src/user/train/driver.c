#include <user/train/driver.h>

#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/terminal_manager.h>
#include <lib/train/track_node.h>
#include <user/train/representer.h>
#include <user/train/reservation_manager.h>

bool DRIVER1_DEF, DRIVER2_DEF;

static tid_t my_tid, tm_tid, sw_tid, rm_tid, tr_tid;

typedef struct TDTrain {
  int num;
  track_node *pos;
  bool stopping;
  path p;
} TDTrain;

void TDTrainInit(TDTrain *train, int num) {
  train->num = num;
  train->pos = NULL;
  train->stopping = 0;
  path_init(&train->p, TRACK);
}


static int StoppingDistance(TDTrain *tr, int gear) {
  TrainModel tm;

  getStoppingDistanceModel(&tm, tr->num);

  return easyInterpolation(&tm, gear)/1000;
}


static void TrainSubscriber(TDTrain *tr) {
  int r;
  tid_t rep_tid, par_tid;
  TrackRequest tr_req;
  TrackData td;

  par_tid = MyParentTid();
  assert(par_tid > 0);
  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);

  tr_req.type = TRR_SUBSCRIBE_DATA;
  tr_req.data.train_num = tr->num;

  while (true) {
    Send(rep_tid, &tr_req, sizeof(tr_req), &td, sizeof(td));
    Send(par_tid, &td.data.tr_train, sizeof(td.data.tr_train), &r, sizeof(r));
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


static bool ShouldStop(path *p, train *raw_train, int stop_dist) {
  int i, dist_to_end;
  track_node *cur;
  track_node *next;

  dist_to_end = 0;
  tr_path_get(&p->ahead, 0, &cur);
  for (i = 1; i < p->ahead.size; ++i) {
    tr_path_get(&p->ahead, i, &next);
    dist_to_end += track_node_dist(cur, next);
    cur = next;
  }

  dist_to_end -= raw_train->curr_pos.off;
  assert(dist_to_end > 0);


  return dist_to_end < stop_dist + 100;
}



#define LOOK_AHEAD 500
static void HandleTrainUpdate(TDTrain *tr, train *raw_train, track_node *end) {
  int r, stop_dist;
  sw_configs sw_cfgs;
  sw_config cfg;

  if (raw_train->curr_pos.pos == tr->pos) {
    return;
  }

  sw_configs_init(&sw_cfgs);
  if (!tr->pos) {
    tr->pos = raw_train->curr_pos.pos;
    path_init(&tr->p, TRACK);
    path_set_destination(&tr->p, tr->pos, end);
    path_generate(&tr->p);
    path_start(&tr->p, tr->pos);
  } else {

    r = path_follow_to(&tr->p, raw_train->curr_pos.pos);
    if (r == -1) {
      // we're off the path
      tr->pos = NULL;

      // TODO: clear out old reservations
      r = Free(rm_tid, tr->num, tr->pos);
      if (r) TMLogStrf(tm_tid, "FREE ALL FAILED\n");
      return;
    }

    if (path_arrived(&tr->p)) {
      TrainCmd(tr->num, 0);
      tr->stopping = false;
      assert(0 && "YEEHAW");
    }

    tr->pos = raw_train->curr_pos.pos;

    stop_dist = StoppingDistance(tr, raw_train->snapshot.cur_gear);
    assert(stop_dist >= 0 && stop_dist <= 2000);

    r = Reserve(rm_tid, tr->num, tr->pos, stop_dist + 100);
    if (r) {
      // failed to get reservation
      TrainCmd(tr->num, 0);
      TMLogStrf(tm_tid, "tr %d FAILED to get reservation", tr->num);
    } else {
      // got the reservation
      r = Free(rm_tid, tr->num, tr->pos);
      if (r) {
        // free failed
        TMLogStrf(tm_tid, "tr %d FAILED free reservation", tr->num);
      }
    }

    if (!tr->stopping && ShouldStop(&tr->p, raw_train, stop_dist)) {
      TrainCmd(tr->num, 3);
      TMLogStrf(tm_tid, "STOPPING tr %d\n");
      tr->stopping = true;
    }
  }

  path_switches_in_next_dist(&tr->p, &sw_cfgs, LOOK_AHEAD);
  while (sw_cfgs.size > 0) {
    sw_configs_pop(&sw_cfgs, &cfg);
    FlipSwitch(&cfg);
  }
}


void TrainDriver(TrainDriverArgs *args) {
  int r;
  tid_t sub_tid;
  TDTrain tr;
  char buf[4096];
  train raw_train;

  TDTrainInit(&tr, args->train_num);

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

  sw_tid = WhoIs(SWITCH_PROVIDER_ID);
  assert(sw_tid > 0);

  my_tid = MyTid();

  CreateArgs(21, &TrainSubscriber, &tr, sizeof(tr));

  TrainCmd(tr.num, 10);

  while (true) {
    Receive(&sub_tid, &raw_train, sizeof(raw_train));
    HandleTrainUpdate(&tr, &raw_train, args->end);

    path_to_str(&tr.p, buf);
    TMPutStrf(tm_tid, "\r%d: %s", tr.num, buf);
    Reply(sub_tid, &r, sizeof(r));
  }
}
