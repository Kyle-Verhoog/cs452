#include <lib/train/track.h>

CIRCULAR_BUFFER_DEF(train_cb, Train, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEF(train_list, Train *, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEF(poss_node_list, track_node *, POSSIBLE_NODE_LIST_SIZE);
CIRCULAR_BUFFER_DEF(update_list, TrackEvent, UPDATE_LIST_SIZE);
CIRCULAR_BUFFER_DEF(ve_list, VirtualEvent, VEVENT_LIST_SIZE);

#define NEXT_NODE(x) x->edge[DIR_AHEAD].dest

void TrackInit(Track *track, track_node *tr) {
  int i;

  train_list_init(&track->lost_trains);
  train_list_init(&track->active_trains);
  update_list_init(&track->updates);
  ve_list_init(&track->vevents);

  track->graph = tr;

  for (i = 0; i < DECODER_SIZE*2; ++i) {
    track->sensors[i].state = 0;
  }

  for (i = 0; i < TRAIN_SIZE; ++i) {
    track->train[i].status = TR_UNINIT;
  }
}

// TODO: replace with proper list datastructure
Train *TrackGetActiveTrain(Track *track, int train_num) {
  int i, r;
  Train *train;

  for (i = 0; i < track->active_trains.size; ++i) {
    r = train_list_get(&track->active_trains, i, &train);
    assert(r == 0);

    if (train->num == train_num)
      return train;
  }

  return 0;
}

// TODO: replace with proper list datastructure
Train *TrackRemoveActiveTrain(Track *track, int train_num) {
  int i, r, size;
  Train *train;

  size = track->active_trains.size;
  for (i = 0; i < size; ++i) {
    r = train_list_pop(&track->active_trains, &train);
    assert(r == 0);

    if (train->num == train_num)
      return train;

    r = train_list_push(&track->active_trains, train);
    assert(r == 0);
  }

  return 0;
}


int TrackGetNextPossibleSensors(Track *track, track_node *node, poss_node_list *pnl) {
  int r;
  assert(node != NULL);

  while (node->type != NODE_SENSOR) {
    if (node->type == NODE_BRANCH) {
      TrackGetNextPossibleSensors(track, node->edge[DIR_STRAIGHT].dest, pnl);
      TrackGetNextPossibleSensors(track, node->edge[DIR_CURVED].dest, pnl);
      return 0;
    }
    else if (node->type == NODE_EXIT) {
      return 0;
    }
    node = node->edge[DIR_AHEAD].dest;
  }

  // printf("%s\n", node->name);
  assert(node->type == NODE_SENSOR);
  r = poss_node_list_push(pnl, node);
  assert(r == 0);
  return 0;
}


//------------------------STATE MUTATORS-----------------------------------------

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
  // TODO:
  // TETRGear event;
  // event.num = cmd_event->arg1;
  // event.newgear = cmd_event->arg2;
}

static void TrackGenerateTrainVEvents(Track *track, Train *train) {
  assert(train->status == TR_KNOWN || train->status == TR_UN_SPEED);
  int i, n, r, dist;
  poss_node_list pnl;
  track_node *sensor;
  assert(train->status != TR_UNINIT);

  poss_node_list_init(&pnl);

  r = TrackGetNextPossibleSensors(track, NEXT_NODE(train->pos), &pnl);

  VirtualEvent ve;
  ve.timeout = -1;
  ve.timestamp = -1;
  ve.type = VE_TR_AT;
  n = pnl.size;

  for (i = 0; i < n; ++i) {
    r = poss_node_list_pop(&pnl, &sensor);
    assert(r == 0);
    dist = track_node_dist(train->pos, sensor);
    if (train->status == TR_KNOWN) {
      ve.timeout = (dist*1000) / train->speed;
      ve.timestamp = ve.timeout + train->sen_ts;
    }
    else if (train->status == TR_UN_SPEED) {
      // ve.timeout = INT_MAX;    // TODO: figure this out
      // ve.timestamp = INT_MAX;
      ve.timestamp = NO_TIMESTAMP;
    }
    else {
      assert(0);
    }
    ve.depend = sensor->num;
    ve.event.train_at.train_num = train->num;
    ve.event.train_at.node = sensor;
    r = ve_list_push(&track->vevents, ve);
    assert(r == 0);
  }
}

#define alpha 90
// recalculate a rough estimate of velocity
static void TrackUpdateTrain(Track *track, Train *train, track_node *new_pos, int ts) {
  int dist, new_speed;

  // train speed and pos are known and the train isn't stopped
  // so we can update the speed
  if (train->status == TR_KNOWN && train->speed > 0) {
    dist = track_node_dist(train->pos, new_pos); // dist is in mm
    assert(dist >= 0 && dist <= 10000);

    assert(0 <= train->speed && train->speed <= 10000);
    assert(0 <= train->sen_ts);

    assert(ts - train->sen_ts != 0);
    new_speed = (dist*1000) / (ts - train->sen_ts); // speed in um/tick

    train->speed = (alpha*new_speed + (100-alpha)*train->speed) / 100;
  }
  // if the speed is unknown and the previous position is not null
  // then we can figure out the speed
  else if (train->status == TR_UN_SPEED && train->pos != NULL) {
    assert(train->speed == -1);
    dist = track_node_dist(train->pos, new_pos); // dist is in mm
    assert(dist >= 0 && dist <= 10000);
    assert(ts - train->sen_ts != 0);
    train->speed = (dist*1000) / ((ts - train->sen_ts)); // speed in um/tick
    train->status = TR_KNOWN;
  }
  else if (train->status == TR_LOST) {
    // skip
  }
  else if (train->speed == 0) {
    return;
  }

  train->sen_ts = ts;
  train->pos = new_pos;
  TrackGenerateTrainVEvents(track, train);
}

void TrackAddLostTrain(Track *track, Train *t) {
  int r;

  assert(t->num >= 0 && t->num <= TRAIN_SIZE);

  t->speed  = -1;
  t->pos    = NULL;
  t->sen_ts = -1;
  t->status = TR_LOST;

  track->train[t->num] = *t;
  r = train_list_push(&track->lost_trains, &track->train[t->num]);
  assert(r == 0);
}

void TrackLocateLostTrain(Track *track, Train *train, int sen_num, int ts) {
  int r;
  track_node *new_pos;

  r = train_list_pop(&track->lost_trains, &train);
  assert(r == 0);

  // set train position to the sensor
  new_pos = &track->graph[sen_num];

  // now we just don't know the train's speed
  train->status = TR_UN_SPEED;
  train->speed = -1;

  TrackUpdateTrain(track, train, new_pos, ts);

  r = train_list_push(&track->active_trains, train);
  assert(r == 0);
}

static void TrackAttemptToLocateTrain(Track *track, int sen_num, int ts) {
  int r;
  Train *t;

  r = train_list_get(&track->lost_trains, 0, &t);
  assert(r == 0);

  // TODO: look in both directions of the node for other possible trains
  //       that this could be
  if (1) {
    TrackLocateLostTrain(track, t, sen_num, ts);
  }
}


//-------------------------EVENT INTERPRETERS----------------------------------

// If we just have an RE, then we just pass the RE through
static void TrackInterpretRE(Track *track, RawEvent *re) {
  RawSensorEvent *se_event;
  RawSwitchEvent *sw_event;
  RawTrainCmdEvent *tr_cmd_event;

  switch (re->type) {
    case RE_SE:
      se_event = &re->event.se_event;
      if (track->lost_trains.size > 0) {
        TrackAttemptToLocateTrain(track, re->event.se_event.id, re->timestamp);
      }
      UpdateSensor(track, se_event->id, se_event->state);
      break;
    case RE_SW:
      sw_event = &re->event.sw_event;
      UpdateSwitch(track, (int)sw_event->sw, sw_event->dir);
      break;
    case RE_TR_CMD:
      tr_cmd_event = &re->event.tr_cmd_event;
      UpdateTrainCmd(track, tr_cmd_event->arg1, tr_cmd_event->arg2);
      break;
    default:
      assert(0);
  }
}

static void TrackInterpretVREVERE(Track *track, EventGroup *grp) {
  int vts, rts, delta;
  Train *train;
  track_node *new_pos;

  vts = grp->ve.timestamp;
  rts = grp->re.timestamp;
  assert(vts <= rts);

  delta = rts - vts;

  assert(IS_VALID_TR_NUM(grp->ve.event.train_at.train_num));
  train = TrackGetActiveTrain(track, grp->ve.event.train_at.train_num);

  new_pos = &track->graph[grp->re.event.se_event.id];

  TrackUpdateTrain(track, train, new_pos, rts);
}

static void TrackInterpretVRERE(Track *track, EventGroup *grp) {
  int vts, rts, delta;
  Train *train;
  track_node *new_pos;

  vts = grp->ve.timestamp;
  rts = grp->re.timestamp;
  // assert(vts >= rts);

  delta = rts - vts;

  assert(IS_VALID_TR_NUM(grp->ve.event.train_at.train_num));
  train = TrackGetActiveTrain(track, grp->ve.event.train_at.train_num);

  new_pos = &track->graph[grp->re.event.se_event.id];

  TrackUpdateTrain(track, train, new_pos, rts);
}

// We didn't get a raw event, so we lose the train for now
static void TrackInterpretVREVE(Track *track, EventGroup *grp) {
  int r, tr_num;
  Train *train;

  tr_num = grp->ve.event.train_at.train_num;
  assert(IS_VALID_TR_NUM(tr_num));

  train = TrackRemoveActiveTrain(track, tr_num);
  assert(train != 0);

  r = train_list_push(&track->lost_trains, train);
  assert(r == 0);
}


int TrackInterpretEventGroup(Track *track, EventGroup *grp) {
  switch (grp->type) {
    case VRE_VE_RE:
      // assert(grp->event.type == RE_SE);
      TrackInterpretVREVERE(track, grp);
      break;
    case VRE_VE:
      // assert(grp->type == RE_SE);
      TrackInterpretVREVE(track, grp);
      break;
    case VRE_RE:
      // assert(grp->type == RE_SE);
      TrackInterpretVRERE(track, grp);
      break;
    case RE:
      TrackInterpretRE(track, &grp->re);
      break;
    default:
      assert(0);
      break;
  }
  return 0;
}

