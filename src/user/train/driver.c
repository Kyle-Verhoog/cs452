#include <user/train/driver.h>

#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/terminal_manager.h>
#include <lib/train/train_defines.h>
#include <user/train/representer.h>
#include <user/train/reservation_manager.h>

bool DRIVER1_DEF, DRIVER2_DEF;

static tid_t my_tid, tm_tid, sw_tid, rm_tid, tr_tid;

typedef struct TDTrain {
  int num;
  int speed;
  int gear;
  track_node *pos;
  track_node *last_pos;
  track_node *last_last_pos;
  track_node *stop_sensor ;
  int delay_dist;
  path p;
} TDTrain;

void TDTrainInit(TDTrain *train, int num) {
  train->num = num;
  train->speed = -1;
  train->pos = NULL;
  train->last_pos = NULL;
  train->last_last_pos = NULL;
  train->gear = 10;
  train->stop_sensor = NULL;
  path_init(&train->p, TRACK);
}

union TDDatum {
  TETRSpeed    speed;
  TETRGear     gear;
  TETRPosition pos;
};

#define SPEED  1
#define POS    2
#define GEAR   3
#define TIMEOUT  4

struct TDData {
  int type;
  union TDDatum event;
};

static int StoppingDistance(TDTrain *train) {
	int stp_dist[] = {0, 0, 0, 0, 0, 0, 3000, 12400, 59950, 149694, 381764, 567824, 759493, 971013, 1238057};
  return stp_dist[train->gear];
}


static int GetLastAvailableSensor(track_node *start, track_node *end, Switch *sw, int min_dist, PossibleSensor *target){
  PossibleSensor pos;
  track_node *n;
  int dist, r;

  target->dist = 0;
  pos.node = start;
  dist = DistanceBetweenNodes(sw, start, end);
  if(dist*1000 < min_dist){
    return -1;
  }

  while(true){
    n = pos.node;
    r = GetNextSensorEXC(sw, n, &pos);
    //assert(r == 0);

    if((dist - pos.dist)*1000 < min_dist){
      break;
    }

    dist -= pos.dist;
    target->dist += pos.dist;
  }

  target->node = n;
  return dist;
}

static void StoreStopSensor(TDTrain *train) {
  int min_dist, dist;
  PossibleSensor ps;
  sw_configs sw_cfgs;
  sw_config cfg;
  Switch swlist[SWITCH_SIZE];

  sw_configs_init(&sw_cfgs);
  path_switches_in_next_dist(&train->p, &sw_cfgs, 500000);

  min_dist = StoppingDistance(train);

  while (sw_cfgs.size > 0) {
    sw_configs_pop(&sw_cfgs, &cfg);
    swlist[cfg.sw->num].state = cfg.state_required == DIR_CURVED ? SW_CURVE : SW_STRAIGHT;
  }

  dist = GetLastAvailableSensor(train->p.start, train->p.end, swlist, min_dist, &ps);

  if (dist > 0) {
    train->delay_dist = (dist*1000) - min_dist;
    train->stop_sensor = ps.node;
  }
  else {
    train->delay_dist = (dist*1000) - min_dist;
    train->stop_sensor = NULL;
  }
}

static void HandleSpeedUpdate(TDTrain *train, int newspeed) {
  train->speed = newspeed;
}

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

static void TrainSpeedSubscriber() {
  int r;
  tid_t rep_tid, par_tid;
  TrackRequest tr_req;
  struct TDData data;

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



static void TrainGearSubscriber() {
  int r;
  tid_t rep_tid, par_tid;
  TrackRequest tr_req;
  struct TDData data;

  data.type = GEAR;

  par_tid = MyParentTid();
  assert(par_tid > 0);
  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);

  tr_req.type = TRR_SUBSCRIBE;
  tr_req.data.type = TE_TR_MOVE;

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


static void TimeoutTask(int *delay) {
  tid_t my_tid, par_tid;
  my_tid = MyTid();
  par_tid = MyParentTid();
  int msg, r;

  msg = TIMEOUT;

  DelayCS(my_tid, *delay + 25); // 25 is *magic*

  Send(par_tid, &msg, sizeof(msg), &r, sizeof(r));
  Exit();
}


#define LOOK_AHEAD 1000
static void HandlePositionUpdate(TDTrain *train, track_node *new_pos) {
  track_node *tn;
  int delay;
  sw_configs sw_cfgs;
  sw_config cfg;

  sw_configs_init(&sw_cfgs);

  if (new_pos == train->stop_sensor && train->speed > 0) {
    delay = train->delay_dist / train->speed;
    CreateArgs(23, &TimeoutTask, &delay, sizeof(delay));
  }

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
    StoreStopSensor(train);
    // r = PathFind(rm_tid, train->num, &train->p);
    path_start(&train->p, train->p.start);
  }

  if (new_pos == train->p.end) {
    TMLogStrf(tm_tid, "ARRIVED pathing to %s\n", train->p.end->reverse->name);
    DelayCS(my_tid, 500);
    path_set_destination(&train->p, new_pos, train->p.end->reverse);
    path_generate(&train->p);
    StoreStopSensor(train);
    path_start(&train->p, train->p.start);
    TrainCmd(train->num, 10);
  }

  train->last_last_pos = train->last_pos;
  train->last_pos = train->pos;
  train->pos = new_pos;

  r = Reserve(rm_tid, train->num, train->pos, StoppingDistance(train)/1000 + 100);

  if (r) {
    TMLogStrf(tm_tid, "RESERVATION CONFLICT\n");
    TrainCmd(train->num, 0);
    DelayCS(my_tid, 10*3*train->gear);
    TMLogStrf(tm_tid, "REVERSING TRAIN\n");
    TrainCmd(train->num, 15);
    TrainCmd(train->num, 10);
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

static void HandleGearUpdate(TDTrain *train, int newgear) {
  train->gear = newgear;
}


static void HandleTimeout(TDTrain *train) {
  TMLogStrf(tm_tid, "STOP TRAIN.. PLEASE\n");
  TrainCmd(train->num, 0);
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
  StoreStopSensor(&train);

  if (r < 0) {
    TMLogStrf(tm_tid, "COULD NOT CALCULATE PATH\n");
    Exit();
  }

  sw_tid = WhoIs(SWITCH_PROVIDER_ID);
  assert(sw_tid > 0);

  my_tid = MyTid();

  Create(21, &TrainPositionSubscriber);
  Create(21, &TrainSpeedSubscriber);
  Create(21, &TrainGearSubscriber);

  TrainCmd(train.num, 10);

  while (true) {
    Receive(&sub_tid, &req, sizeof(req));

    switch (req.type) {
      case POS:
        if (train.num == req.event.pos.num) {
          HandlePositionUpdate(&train, req.event.pos.node);
        }
        break;
      case GEAR:
        if (train.num == req.event.gear.num) {
          HandleGearUpdate(&train, req.event.gear.newgear);
        }
        break;
      case SPEED:
        if (train.num == req.event.speed.num) {
          HandleSpeedUpdate(&train, req.event.speed.new);
        }
        break;
      case TIMEOUT:
        HandleTimeout(&train);
        break;
      default: assert(0);
    }

    path_to_str(&train.p, buf);
    TMPutStrf(tm_tid, "\r%d: %s", train.num, buf);
    Reply(sub_tid, &r, sizeof(r));
  }
}
