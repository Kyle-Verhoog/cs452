#include <lib/train/track.h>

CIRCULAR_BUFFER_DEF(train_cb, Train, TRAIN_LIST_SIZE);
EXT_CIRCULAR_BUFFER_DEF(train_list, Train *, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEF(update_list, TrackEvent, UPDATE_LIST_SIZE);
CIRCULAR_BUFFER_DEF(ve_list, VirtualEvent, VEVENT_LIST_SIZE);

#define NEXT_NODE(x) x->edge[DIR_AHEAD].dest
#define NEXT_DIST(x) x->edge[DIR_AHEAD].dist

void TrackInit(Track *track, track_node *tr) {
  int i;

  train_list_init(&track->lost_trains);
  train_list_init(&track->active_trains);
  update_list_init(&track->updates);
  ve_list_init(&track->vevents);

  track->graph = tr;
  track->ntrains = 0;

  for (i = 0; i < DECODER_SIZE*2; ++i) {
    track->sensors[i].state = 0;
  }

  for (i = 0; i < NUM_TRAINS; ++i) {
    track->train[i].status = TR_UNINIT;
    track->train[i].pos = NULL;
    ev_wm_init(&track->train[i].wm);
  }

  for (i = 0; i < TRAIN_MAX; ++i) {
    track->tmap[i] = -1;
  }
}


//-----------------------------EVENT GENERATORS--------------------------------

static void TrackGenerateTrainSpeedTEvent(Track *track, Train *train, int old, int new) {
  int r;
  TrackEvent event;
  event.type = TE_TR_SPEED;
  event.event.tr_speed.num = train->num;
  event.event.tr_speed.old = old;
  event.event.tr_speed.new = new;
  r = update_list_push(&track->updates, event);
  assert(r == 0);
}

static void TrackGenerateTrainStatusTEvent(Track *track, Train *train, TrainStatus old, TrainStatus new) {
  int r;
  TrackEvent event;
  event.type = TE_TR_STATUS;
  event.event.tr_status.num = train->num;
  event.event.tr_status.old = old;
  event.event.tr_status.new = new;
  r = update_list_push(&track->updates, event);
  assert(r == 0);
}

static void TrackGenerateTrainPositionTEvent(Track *track, Train *train) {
  int r;
  TrackEvent event;
  event.type = TE_TR_POSITION;
  event.event.tr_pos.num = train->num;
  event.event.tr_pos.node = train->pos;
  r = update_list_push(&track->updates, event);
  assert(r == 0);
}

static void TrackAddVEvent(Track *track, Train *train, track_node *tn, VirtualEvent *ve) {
  int r;
  ev_wm_next_key(&train->wm);
  // printf("%d\n", track->key);
  ve->key = train->wm.key;
  r = ve_list_push(&track->vevents, *ve);
  assert(r == 0);

  r = ev_wm_add_to_window(&train->wm, ve->key, train->pos);
  // assertf(r == 0, "%d, %d %d %d %d %d %d\r\n", r, ve->key, train->window.start, train->window.end, train->window.size, train->window.unexp_size);
  assert(r == 0);
}

static void TrackGenerateUnknownSpeedTrainVEvents(Track *track, Train *train) {
  assert(train->status == TR_UN_SPEED);
  int r, s, next_dist;
  poss_node_list pnl;
  PossibleSensor sensor;
  assert(train->status != TR_UNINIT);

  poss_node_list_init(&pnl);

  r = next_poss_sensors(NEXT_NODE(train->pos), NEXT_DIST(train->pos), &pnl);

  VirtualEvent ve;
  ve.type = VE_TR_AT;

  s = pnl.size;
  while (pnl.size > 0) {
    r = poss_node_list_pop(&pnl, &sensor);
    assert(r == 0);
    // use the last known speed if it exists to try to guess a timestamp
    if (train->speed > 0 && train->gear > 0) {
      next_dist = NEXT_DIST(sensor.node);
      ve.timestamp = ((sensor.dist*1000) / train->speed) + train->sen_ts;
      ve.timeout = (((next_dist/3)*1000) / train->speed);
    }
    else {
      ve.timeout = -1;
      ve.timestamp = 500 + train->sen_ts;
    }
    ve.depend = sensor.node->num;
    ve.event.train_at.train_num = train->num;
    ve.event.train_at.node = sensor.node;

    TrackAddVEvent(track, train, sensor.node, &ve);
  }

  // if we added events, then advance the window
  if (s > 0) {
    r = ev_wm_next_window(&train->wm);
    assert(r == 0);
  }
}

static void TrackGenerateKnownTrainVEvents(Track *track, Train *train) {
  assert(train->status == TR_KNOWN);
  int s, r, dist, next_dist;
  poss_node_list pnl;
  PossibleSensor sensor;
  assert(train->status != TR_UNINIT);

  poss_node_list_init(&pnl);

  r = next_poss_sensors(NEXT_NODE(train->pos), NEXT_DIST(train->pos), &pnl);

  VirtualEvent ve;
  ve.type = VE_TR_AT;

  s = pnl.size;
  while (pnl.size > 0) {
    r = poss_node_list_pop(&pnl, &sensor);
    assert(r == 0);
    dist = sensor.dist;
    next_dist = NEXT_DIST(sensor.node);

    ve.timestamp = ((dist*1000) / train->speed) + train->sen_ts;
    ve.timeout = (((next_dist/3)*1000) / train->speed);
    ve.depend = sensor.node->num;
    ve.event.train_at.train_num = train->num;
    ve.event.train_at.node = sensor.node;

    TrackAddVEvent(track, train, sensor.node, &ve);
  }
  if (s > 0) {
    r = ev_wm_next_window(&train->wm);
    assert(r == 0);
  }
}



//------------------------------STATE MUTATORS---------------------------------

static void UpdateSensor(Track *track, int sen_num, int state) {
  int r;
  TrackEvent event;

  assert(sen_num >= 0 && sen_num < DECODER_SIZE*2*16);
  assert(state == 0 || state == 1);
  track->sensors[sen_num].state = state;

  event.type = TE_SE_CHANGE;
  event.event.se_change.num = sen_num;
  event.event.se_change.state = state;
  r = update_list_push(&track->updates, event);
  assert(r == 0);
}

static void UpdateSwitch(Track *track, int sw_num, int state) {
  int r;

  assert((NORM_SW_LOW <= sw_num && sw_num <= NORM_SW_HIGH) ||
      (SPEC_SW_LOW <= sw_num && sw_num <= SPEC_SW_HIGH));

  TrackEvent event;
  event.type = TE_SW_CHANGE;
  event.event.sw_change.num = sw_num;
  event.event.sw_change.newdir = state;
  r = update_list_push(&track->updates, event);
  assert(r == 0);
}

static void UpdateTrainCmd(Track *track, int tr_num, int cmd) {
  int r;
  TrackEvent event;
  event.type = TE_TR_MOVE;
  event.event.tr_gear.num = tr_num;
  event.event.tr_gear.newgear = cmd;
  if (cmd == 15) {  //Reverse
    track->train[track->tmap[tr_num]].pos = track->train[track->tmap[tr_num]].pos->reverse;
    TrackGenerateTrainPositionTEvent(track, &track->train[track->tmap[tr_num]]);
  }
  r = update_list_push(&track->updates, event);
  assert(r == 0);
}

static void InitTrainCmd(Track *track, int tr_num, int node){
  int r;
  Train *train;

  assert(tr_num >= 0 && tr_num < TRAIN_SIZE);
  track->tmap[tr_num] = track->ntrains++;

  train = &track->train[track->tmap[tr_num]];
  assert(train->status == TR_UNINIT);

  ev_wm_init(&train->wm);

  train->num    = tr_num;
  train->speed  = 0;
  train->gear   = 0;
  train->pos    = &track->graph[node];
  train->sen_ts = 0;
  train->status = TR_LOST;

  r = train_list_push(&track->lost_trains, train);
  assert(r == 0);

  TrackGenerateTrainStatusTEvent(track, train, TR_UNINIT, TR_LOST);
  TrackGenerateTrainPositionTEvent(track, train);
}

#define alpha 90
// recalculate a rough estimate of velocity
static void TrackUpdateKnownTrain(Track *track, Train *train, track_node *new_pos, int ts) {
  int dist, old_speed, new_speed;
  assert(train->status == TR_KNOWN);

  old_speed = train->speed;

  if (old_speed > 0) {
    // TODO: the below will not work across branches?
    // dist = track_node_dist(train->pos, new_pos); // dist is in mm
    // TODO: replace this with something better?
    dist = dist_to_node(train->pos, new_pos);
    assert(dist > 0 && dist <= 10000);

    assert(0 <= old_speed && old_speed <= 10000);
    assert(0 <= train->sen_ts);

    assert(ts - train->sen_ts != 0);
    if (ts-train->sen_ts == 0)
      new_speed = train->speed;
    else
      new_speed = (dist*1000) / (ts - train->sen_ts); // speed in um/tick

    train->speed = (alpha*new_speed + (100-alpha)*old_speed) / 100;
    train->sen_ts = ts;
    train->pos = new_pos;
    TrackGenerateKnownTrainVEvents(track, train);
    TrackGenerateTrainSpeedTEvent(track, train, old_speed, new_speed);
    TrackGenerateTrainPositionTEvent(track, train);
  }
  else if (train->speed == 0) {
    train->sen_ts = ts;
    train->pos = new_pos;
  }
  else {
    assert(train->speed > 0);
  }
}

// UN_SPEED -> KNOWN
static void TrackUpdateUnknownSpeedTrain(Track *track, Train *train, track_node *new_pos, int ts) {
  int dist;
  assert(train->status == TR_UN_SPEED && train->pos != NULL);

  // if the speed is unknown and the previous position is not null then we can
  // figure out the speed
  assert(train->speed == -1);
  dist = dist_to_node(train->pos, new_pos);
  assert(dist >= 0 && dist <= 10000);
  assert(ts - train->sen_ts != 0);
  train->speed = (dist*1000) / ((ts - train->sen_ts)); // speed in um/tick
  train->status = TR_KNOWN;
  train->sen_ts = ts;
  train->pos = new_pos;
  TrackGenerateKnownTrainVEvents(track, train);
  TrackGenerateTrainStatusTEvent(track, train, TR_UN_SPEED, TR_KNOWN);
  TrackGenerateTrainSpeedTEvent(track, train, -1, train->speed);
  TrackGenerateTrainPositionTEvent(track, train);
}

// LOST -> UN_SPEED
static void TrackUpdateLostTrain(Track *track, Train *train, track_node *new_pos, int ts) {
  assert(train->status == TR_LOST);

  // now we just don't know the train's speed
  train->status = TR_UN_SPEED;
  train->speed = -1;

  train->sen_ts = ts;
  train->pos = new_pos;
  TrackGenerateTrainStatusTEvent(track, train, TR_LOST, TR_UN_SPEED);
  TrackGenerateUnknownSpeedTrainVEvents(track, train);
  TrackGenerateTrainPositionTEvent(track, train);
}

// UNINIT -> TR_LOST
void TrackAddTrain(Track *track, Train *tr) {
  int r;
  Train *train;

  assert(tr->num >= 0 && tr->num <= TRAIN_SIZE);
  track->tmap[tr->num] = track->ntrains++;

  train = &track->train[track->tmap[tr->num]];
  assert(train->status == TR_UNINIT);

  ev_wm_init(&train->wm);

  train->num    = tr->num;
  train->speed  = -1;
  train->gear   = tr->gear;
  train->pos    = NULL;
  train->sen_ts = -1;
  train->status = TR_LOST;

  r = train_list_push(&track->lost_trains, train);
  assert(r == 0);

  TrackGenerateTrainStatusTEvent(track, train, TR_UNINIT, TR_LOST);
}

// * -> TR_LOST
static void TrackLoseTrain(Track *track, Train *train) {
  int r;
  TrainStatus old_status;

  assert(train == &track->train[track->tmap[train->num]]);
  old_status = train->status;
  train->status = TR_LOST;

  // r = train_list_rem(&track->active_trains, train);
  // assert(r == 0);

  r = train_list_push(&track->lost_trains, train);
  assert(r == 0);
  TrackGenerateTrainStatusTEvent(track, train, old_status, TR_LOST);
}


#define NEARBY_SENSOR_DEPTH 2
static Train *TrackAttemptToLocateTrain(Track *track, track_node *node, int ts) {
  int r, i, n;
  Train *train;

  n = track->lost_trains.size;

  // loop over the lost trains checking to see which is nearby the sensor
  for (i = 0; i < n; ++i) {
    r = train_list_pop(&track->lost_trains, &train);
    assert(r == 0);

    if (train->gear > 0 && (!train->pos || node_nearby_sd(train->pos, node, NEARBY_SENSOR_DEPTH))) {
      TrackUpdateLostTrain(track, train, node, ts);
      return train;
    }

    r = train_list_push(&track->lost_trains, train);
    assert(r == 0);
  }

  return NULL;
}

// checks backwards from a sensor looking for a train
static Train *TrackCheckSensorForFastTrain(Track *track, track_node *sensor) {
  int i;
  Train *train;
  assert(sensor->type == NODE_SENSOR);

  // flip the sensor to look in the reverse direction
  sensor = sensor->reverse;

  // TODO(1): this is pretty inefficient, we're searching again and again for each
  // train position rather than searching once and checking for a match with a
  // train position

  // this can be improved by writing a BFS and checking each node to see if it
  // matches a train's position

  // TODO(2): we also probably want to check some other data about the train to
  // ensure that this is not a false positive sensor reading occuring in front
  // of a train else we are going to advance the train to a position in which it
  // is not actually
  for (i = 0; i < track->ntrains; ++i) {
    train = &track->train[i];

    // break since it is already at the sensor
    if (train->pos == sensor->reverse) {
      break;
    }

    if (node_nearby_sd(sensor, train->pos->reverse, 2))
      return train;
  }

  return NULL;
}

// * -> TR_KNOWN
// used to advance a train when it is ahead of schedule and our window timeout is
// off
static void TrackUpdateFastTrain(Track *track, Train *train, track_node *sensor, int ts) {
  int dist;
  // assert(train->status == TR_KNOWN || train->status == TR_UN_SPEED);

  dist = dist_to_node(train->pos, sensor);
  assert(dist >= 0 && dist <= 10000);
  assert(ts - train->sen_ts != 0);
  train->speed = (dist*1000) / ((ts - train->sen_ts)); // speed in um/tick
  train->status = TR_KNOWN;
  train->sen_ts = ts;
  train->pos = sensor;
  TrackGenerateKnownTrainVEvents(track, train);
  TrackGenerateTrainStatusTEvent(track, train, TR_UN_SPEED, TR_KNOWN);
  TrackGenerateTrainSpeedTEvent(track, train, -1, train->speed);
  TrackGenerateTrainPositionTEvent(track, train);
}


//-------------------------EVENT INTERPRETERS----------------------------------


static void TrackHandlePotentialTrainAtSensorRaw(Track *track, RawSensorEvent *se, int ts) {
  Train *train;
  track_node *sensor;
  int r;
  train = NULL;

  sensor = &track->graph[se->id];

  // shortcut if the train is already on this sensor

  if (track->lost_trains.size > 0) {
    train = TrackAttemptToLocateTrain(track, sensor, ts);
  }

  // if we haven't located a lost train, then try detecting a fast train
  if (!train) {
    // Handle trains that trigger events that have not been generated yet
    // (happens when our estimate for the window timeout is too long eg. train acc)
    train = TrackCheckSensorForFastTrain(track, sensor);
    if (train) {
      r = ev_wm_delete_all(&train->wm);
      assert(r == 0);
      TrackUpdateFastTrain(track, train, sensor, ts);
    }
  }
}

static void TrackHandleTrainCmd(Track *track, int train_num, int cmd) {
  Train t;
  if (track->tmap[train_num] == -1) {
    t.num = train_num;
    t.gear = cmd;
    TrackAddTrain(track, &t);
  }
}

// If we just have an RE, then we just pass the RE through
static void TrackHandleRawEvent(Track *track, RawEvent *re, bool check_trains) {
  RawSensorEvent *se_event;
  RawSwitchEvent *sw_event;
  RawTrainCmdEvent *tr_cmd_event;

  switch (re->type) {
    case RE_SE:
      se_event = &re->event.se_event;

      // only check trains that have a high state
      if (check_trains && se_event->state) {
        TrackHandlePotentialTrainAtSensorRaw(track, se_event, re->timestamp);
      }
      UpdateSensor(track, se_event->id, se_event->state);
      break;
    case RE_SW:
      sw_event = &re->event.sw_event;
      UpdateSwitch(track, (int)sw_event->sw, sw_event->dir);
      break;
    case RE_TR_CMD:
      tr_cmd_event = &re->event.tr_cmd_event;
      TrackHandleTrainCmd(track, tr_cmd_event->arg1, tr_cmd_event->arg2);
      UpdateTrainCmd(track, tr_cmd_event->arg1, tr_cmd_event->arg2);
      break;
    case RE_TR_INIT:
      tr_cmd_event = &re->event.tr_cmd_event;
      InitTrainCmd(track, tr_cmd_event->arg1, tr_cmd_event->arg2);
      break;
    default:
      assert(0);
  }
}

static void TrackHandleTrainAtSensor(Track *track, EventGroup *grp) {
  int r, train_num, ekey, vts, rts, delta;
  Train *train;
  track_node *new_pos;

  vts = grp->ve.timestamp;
  rts = grp->re.timestamp;
  delta = rts - vts;

  ekey = grp->ve.key;

  train_num = grp->ve.event.train_at.train_num;
  assert(IS_VALID_TR_NUM(train_num));
  train = &track->train[track->tmap[train_num]];

  r = ev_wm_res_to_window(&train->wm, ekey, HIT);
  if (r == -1) {
    // assert(0 && "window does not exist");
    return;
  }
  // check that the train status is not lost for the edge case where:
  // eg. D12 is broken and train runs over E11
  // the train is lost and then gets a window double hit
  else if (r == 2 && train->status != TR_LOST) {
    // we got more than 1 sensor hit for a window lose the train
    // and invalidate newer windows
    train->pos = ev_wm_get_window_tn(&train->wm, ekey);
    ev_wm_invalidate_after(&train->wm, ekey);
    if(train->status == TR_LOST){
      assert(0 && "RE-LOSING TRAIN");
    }
    TrackLoseTrain(track, train);
    assert(train->status == TR_LOST);
  }

  r = ev_wm_delete_if_complete(&train->wm, ekey);
  if (r == -1) {
    // TMLogStrf(tm_tid, "EVDEL: window does not exist\n");
  }
  else if (r == -2) {
  }
  else if (r == -3) {
    // assert(0);
    // TMLogStrf(tm_tid, "EVDEL: REMOVE ERROR\n");
  }
  else if (r == -4) {
    assert(0);
    // TMLogStrf(tm_tid, "EVDEL: avail_windows FULL\n");
  }

  if (train->status == TR_KNOWN) {
    new_pos = &track->graph[grp->re.event.se_event.id];
    TrackUpdateKnownTrain(track, train, new_pos, rts);
  }
  else if (train->status == TR_UN_SPEED) {
    new_pos = &track->graph[grp->re.event.se_event.id];
    TrackUpdateUnknownSpeedTrain(track, train, new_pos, rts);
  }
  else if (train->status == TR_LOST) {
  }
  else {
    assert(0);
  }
}


// We didn't get a raw event, so we lose the train for now
static void TrackHandleTrainAtTimeout(Track *track, EventGroup *grp) {
  int r, train_num, ekey;
  Train *train;

  ekey = grp->ve.key;

  train_num = grp->ve.event.train_at.train_num;
  assert(IS_VALID_TR_NUM(train_num));
  train = &track->train[track->tmap[train_num]];

  r = ev_wm_res_to_window(&train->wm, ekey, TIMEOUT);

  if (r == -1) {
    // window does not exist
    // assert(0 && "window does not exist");
    return;
  }
  else if (r == 1) {
    // all events timed out, reset back to the prev node, invalidate events
    // after the node and lose the train
    assert(train->status != TR_LOST);
    train->pos = ev_wm_get_window_tn(&train->wm, ekey);
    ev_wm_invalidate_after(&train->wm, ekey);
    TrackLoseTrain(track, train);
  }

  ev_wm_delete_if_complete(&train->wm, ekey);
  if (r == -1) {
    // TMLogStrf(tm_tid, "EVDEL: window does not exist\n");
  }
  else if (r == -2) {
  }
  else if (r == -3) {
    assert(0);
    // TMLogStrf(tm_tid, "EVDEL: REMOVE ERROR\n");
  }
  else if (r == -4) {
    assert(0);
    // TMLogStrf(tm_tid, "EVDEL: avail_windows FULL\n");
  }
}


int TrackInterpretEventGroup(Track *track, EventGroup *grp) {
  switch (grp->type) {
    case VRE_VE_RE:
    case VRE_RE:
      TrackHandleTrainAtSensor(track, grp);
      // pass on the raw event, but don't update trains
      TrackHandleRawEvent(track, &grp->re, false);
      break;
    case VRE_VE:
      TrackHandleTrainAtTimeout(track, grp);
      break;
    case RE:
      TrackHandleRawEvent(track, &grp->re, true);
      break;
    default:
      assert(0);
      break;
  }
  return 0;
}
