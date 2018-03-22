#include <lib/train/track.h>

CIRCULAR_BUFFER_DEF(train_cb, Train, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEF(train_list, Train *, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEF(poss_node_list, PossibleSensor, POSSIBLE_NODE_LIST_SIZE);
CIRCULAR_BUFFER_DEF(update_list, TrackEvent, UPDATE_LIST_SIZE);
CIRCULAR_BUFFER_DEF(ve_list, VirtualEvent, VEVENT_LIST_SIZE);
CIRCULAR_BUFFER_DEF(bfs_q, track_node *, BFS_Q_SIZE);

#define NEXT_NODE(x) x->edge[DIR_AHEAD].dest
#define NEXT_DIST(x) x->edge[DIR_AHEAD].dist

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
    track->train[i].num = i;
    track->train[i].status = TR_UNINIT;
    track->train[i].pos = NULL;
    track->train[i].prev_pos = NULL;
  }

  track->key = 0;
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

// TODO: recursion bad
int GetDistanceBetweenNodes(track_node *node, track_node *dest, int dist, int d) {
  // printf("%s\n", node->name);
  if (!d-- || node == dest) return dist;

  int r;
  if (node->type == NODE_BRANCH) {
    r = GetDistanceBetweenNodes(node->edge[DIR_STRAIGHT].dest, dest, dist + node->edge[DIR_STRAIGHT].dist, d);
    if (r > 0) return r;
    return GetDistanceBetweenNodes(node->edge[DIR_CURVED].dest, dest, dist + node->edge[DIR_CURVED].dist, d);
  }
  else if (node->type == NODE_EXIT) {
    assert(0);
    return 0;
  }

  return GetDistanceBetweenNodes(NEXT_NODE(node), dest, dist+NEXT_DIST(node), d);
}

int FindDistToNode(track_node *node, track_node *dest) {
  int r, d;
  int dist[TRACK_MAX];
  bfs_q q;
  track_node *next;
  bfs_q_init(&q);

  dist[node->id] = 0;
  r = bfs_q_push(&q, node);
  d = BFS_Q_SIZE;

  while (q.size > 0 && d-- > 0) {
    r = bfs_q_pop(&q, &node);
    assert(r == 0);

    if (node == dest)
      return dist[node->id];

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      r = bfs_q_push(&q, next);
      dist[next->id] = node->edge[DIR_CURVED].dist + dist[node->id];
      assert(r == 0);
      next = node->edge[DIR_STRAIGHT].dest;
      r = bfs_q_push(&q, node->edge[DIR_STRAIGHT].dest);
      dist[next->id] = node->edge[DIR_STRAIGHT].dist + dist[node->id];
      assert(r == 0);
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      r = bfs_q_push(&q, node->edge[DIR_AHEAD].dest);
      dist[next->id] = node->edge[DIR_AHEAD].dist + dist[node->id];
      assert(r == 0);
    }
  }

  return -1;
}


int GetNextPossibleSensors(track_node *node, int dist, poss_node_list *pnl) {
  int r;
  PossibleSensor sensor;
  assert(node != NULL);
  assert(dist > 0 && dist < 10000);

  while (node->type != NODE_SENSOR) {
    if (node->type == NODE_BRANCH) {
      // TODO: replace with stack data structure once we have one
      GetNextPossibleSensors(node->edge[DIR_STRAIGHT].dest, dist + node->edge[DIR_STRAIGHT].dist, pnl);
      GetNextPossibleSensors(node->edge[DIR_CURVED].dest, dist + node->edge[DIR_CURVED].dist, pnl);
      return 0;
    }
    else if (node->type == NODE_EXIT) {
      return 0;
    }

    dist += node->edge[DIR_AHEAD].dist;
    node = node->edge[DIR_AHEAD].dest;
  }

  sensor.node = node;
  sensor.dist = dist;
  r = poss_node_list_push(pnl, sensor);
  assert(r == 0);
  return 0;
}

int GetNextSensorINC(Switch *sw, track_node *n, PossibleSensor *pos){
  pos->dist = 0;
  track_node *node = n;

  while(node->type != NODE_SENSOR){
    if(node->type == NODE_EXIT){
      return -1;
    }
    else if(node->type == NODE_BRANCH){
      if(sw[node->num].state == SW_STRAIGHT){
        pos->dist += node->edge[DIR_STRAIGHT].dist;
        node = node->edge[DIR_STRAIGHT].dest;
      }
      else{
        pos->dist += node->edge[DIR_CURVED].dist;
        node = node->edge[DIR_CURVED].dest;
      }
    }
    else{
      pos->dist += node->edge[DIR_AHEAD].dist;
      node = node->edge[DIR_AHEAD].dest;
    }
  }

  pos->node = node;
  return 0;
}

int GetNextSensorEXC(Switch *sw, track_node *n, PossibleSensor *pos){
  pos->dist = 0;
  track_node *node = n;

  do{
    if(node->type == NODE_EXIT){
      return -1;
    }
    else if(node->type == NODE_BRANCH){
      if(sw[node->num].state == SW_STRAIGHT){
        pos->dist += node->edge[DIR_STRAIGHT].dist;
        node = node->edge[DIR_STRAIGHT].dest;
      }
      else{
        pos->dist += node->edge[DIR_CURVED].dist;
        node = node->edge[DIR_CURVED].dest;
      }
    }
    else{
      pos->dist += node->edge[DIR_AHEAD].dist;
      node = node->edge[DIR_AHEAD].dest;
    }
  }while(node->type != NODE_SENSOR);

  pos->node = node;
  return 0;
}

int GetPrevSensorINC(Switch *sw, track_node *n, PossibleSensor *pos){
  return GetNextSensorINC(sw, n->reverse, pos);
}

int GetPrevSensorEXC(Switch *sw, track_node *n, PossibleSensor *pos){
  return GetNextSensorEXC(sw, n->reverse, pos);
}

int DistanceBetweenNodes(Switch *sw, track_node *start, track_node *end){
  int dist = 0;
  track_node *n = start;

  while(n != end){
    if(n->type == NODE_EXIT){
     return -1;
    }
    else if(n->type == NODE_BRANCH){
     if(sw[n->num].state == SW_STRAIGHT){
       dist += n->edge[DIR_STRAIGHT].dist;
       n = n->edge[DIR_STRAIGHT].dest;
     }
     else{
       dist += n->edge[DIR_CURVED].dist;
       n = n->edge[DIR_CURVED].dest;
     }
    }
    else{
     dist += n->edge[DIR_AHEAD].dist;
     n = n->edge[DIR_AHEAD].dest;
    }

    assert(dist > 0 && dist < 50000); //Probably inf loop
  }

  return dist;
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

static void TrackAddVEvent(Track *track, Train *train, VirtualEvent *ve) {
  int r;
  track->key = ev_window_inc_key(track->key);
  ve->key = track->key;
  r = ve_list_push(&track->vevents, *ve);
  r = ev_window_add_key(&train->window, ve->key);
  // assertf(r == 0, "%d, %d %d %d %d %d %d\r\n", r, ve->key, train->window.start, train->window.end, train->window.size, train->window.unexp_size);
  assert(r == 0);
}

static void TrackGenerateUnknownSpeedTrainVEvents(Track *track, Train *train) {
  assert(train->status == TR_UN_SPEED);
  int r;
  poss_node_list pnl;
  PossibleSensor sensor;
  assert(train->status != TR_UNINIT);

  poss_node_list_init(&pnl);

  r = GetNextPossibleSensors(NEXT_NODE(train->pos), NEXT_DIST(train->pos), &pnl);

  VirtualEvent ve;
  ve.type = VE_TR_AT;

  if (pnl.size > 0) {
    // we're sending new events, so invalidate the old ones
    ev_window_shift_all(&train->window);
  }

  while (pnl.size > 0) {
    r = poss_node_list_pop(&pnl, &sensor);
    assert(r == 0);
    ve.timeout = -1;
    ve.timestamp = 10000 + train->sen_ts;
    ve.depend = sensor.node->num;
    ve.event.train_at.train_num = train->num;
    ve.event.train_at.node = sensor.node;

    TrackAddVEvent(track, train, &ve);
  }
}

static void TrackGenerateKnownTrainVEvents(Track *track, Train *train) {
  assert(train->status == TR_KNOWN);
  int r, dist, next_dist;
  poss_node_list pnl;
  PossibleSensor sensor;
  assert(train->status != TR_UNINIT);

  poss_node_list_init(&pnl);

  r = GetNextPossibleSensors(NEXT_NODE(train->pos), NEXT_DIST(train->pos), &pnl);

  VirtualEvent ve;
  ve.type = VE_TR_AT;

  if (pnl.size > 0) {
    // we're sending new events, so invalidate the old ones
    ev_window_shift_all(&train->window);
  }

  while (pnl.size > 0) {
    r = poss_node_list_pop(&pnl, &sensor);
    assert(r == 0);
    dist = sensor.dist;
    next_dist = NEXT_DIST(sensor.node);

    ve.timestamp = ((dist*1000) / train->speed) + train->sen_ts;
    ve.timeout = ((next_dist*1000) / train->speed);
    ve.depend = sensor.node->num;
    ve.event.train_at.train_num = train->num;
    ve.event.train_at.node = sensor.node;

    TrackAddVEvent(track, train, &ve);
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
  r = update_list_push(&track->updates, event);
  assert(r == 0);
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
    dist = FindDistToNode(train->pos, new_pos);
    assert(dist > 0);
    assert(dist >= 0 && dist <= 10000);

    assert(0 <= old_speed && old_speed <= 10000);
    assert(0 <= train->sen_ts);

    assert(ts - train->sen_ts != 0);
    if (ts-train->sen_ts == 0)
      new_speed = train->speed;
    else
      new_speed = (dist*1000) / (ts - train->sen_ts); // speed in um/tick

    train->speed = (alpha*new_speed + (100-alpha)*old_speed) / 100;
    train->sen_ts = ts;
    train->prev_pos = train->pos;
    train->pos = new_pos;
    TrackGenerateKnownTrainVEvents(track, train);
    TrackGenerateTrainSpeedTEvent(track, train, old_speed, new_speed);
    TrackGenerateTrainPositionTEvent(track, train);
  }
  else if (train->speed == 0) {
    train->sen_ts = ts;
    train->prev_pos = train->pos;
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
  // dist = track_node_dist(train->pos, new_pos); // dist is in mm
  dist = FindDistToNode(train->pos, new_pos);
  assert(dist >= 0 && dist <= 10000);
  assert(ts - train->sen_ts != 0);
  train->speed = (dist*1000) / ((ts - train->sen_ts)); // speed in um/tick
  train->status = TR_KNOWN;
  train->sen_ts = ts;
  train->prev_pos = train->pos;
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
  train->prev_pos = train->pos;
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
  train = &track->train[tr->num];
  assert(train->status == TR_UNINIT);

  ev_window_init(&train->window);

  train->speed  = -1;
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

  assert(train== &track->train[train->num]);
  old_status = train->status;
  train->status = TR_LOST;

  TrackRemoveActiveTrain(track, train->num);

  r = train_list_push(&track->lost_trains, train);
  assert(r == 0);
  TrackGenerateTrainStatusTEvent(track, train, old_status, TR_LOST);
}

static void TrackLocateLostTrain(Track *track, Train *train, track_node *sen, int ts) {
  int r;

  TrackUpdateLostTrain(track, train, sen, ts);

  r = train_list_push(&track->active_trains, train);
  assert(r == 0);
}

/*
static bool TrainNearby(track_node *node, track_node *dest, int d) {
  if (!d--) return false;
  if (node == dest) return true;

  int r;
  if (node->type == NODE_BRANCH) {
    // TODO: replace with stack data structure once we have one
    r = TrainNearby(node->edge[DIR_STRAIGHT].dest, dest, d);
    if (r) return true;
    return TrainNearby(node->edge[DIR_CURVED].dest, dest, d);
  }
  else if (node->type == NODE_EXIT) {
    return false;
  }

  return TrainNearby(NEXT_NODE(node), dest, d);
}
*/

static bool TrainNearby(track_node *node, track_node *dest) {
  int r, d, b;
  bfs_q q;
  bfs_q_init(&q);

  r = bfs_q_push(&q, node);
  d = BFS_Q_SIZE;
  b = 0;

  while (q.size > 0 && d-- > 0 && b < 3) {
    r = bfs_q_pop(&q, &node);
    assert(r == 0);

    if (node == dest)
      return true;

    if (node->type == NODE_BRANCH) {
      r = bfs_q_push(&q, node->edge[DIR_CURVED].dest);
      assert(r == 0);
      r = bfs_q_push(&q, node->edge[DIR_STRAIGHT].dest);
      assert(r == 0);
      b++;
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      r = bfs_q_push(&q, node->edge[DIR_AHEAD].dest);
      assert(r == 0);
    }
  }

  return false;
}

/*
// BFS
static int GetDistanceBetweenNodes(track_node *node, track_node *dest) {
  int r, d, dist;
  bfs_q q;
  bfs_q_init(&q);

  r = bfs_q_push(&q, node);
  d = BFS_DEPTH;

  dist = 0;

  while (q.size > 0 && d-- > 0) {
    r = bfs_q_pop(&q, &node);
    assert(r == 0);

    if (node == dest)
      return dist;

    if (node->type == NODE_BRANCH) {
      r = bfs_q_push(&q, node->edge[DIR_CURVED].dest);
      assert(r == 0);
      r = bfs_q_push(&q, node->edge[DIR_STRAIGHT].dest);
      assert(r == 0);
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      r = bfs_q_push(&q, node->edge[DIR_AHEAD].dest);
      assert(r == 0);
    }
  }

  return -1;
}*/


static Train *TrackAttemptToLocateTrain(Track *track, int sen_num, int ts) {
  int r, i, n;
  Train *train;
  track_node *node;

  n = track->lost_trains.size;
  node = &track->graph[sen_num];

  // loop over the lost trains checking to see which is nearby the sensor
  for (i = 0; i < n; ++i) {
    r = train_list_pop(&track->lost_trains, &train);
    assert(r == 0);

    if (!train->pos || TrainNearby(train->pos, node)) {
      TrackLocateLostTrain(track, train, node, ts);
      return train;
    }

    r = train_list_push(&track->lost_trains, train);
    assert(r == 0);
  }

  return NULL;
}


//-------------------------EVENT INTERPRETERS----------------------------------


static void TrackHandleTrainAtSensorRaw(Track *track, RawSensorEvent *se, int ts) {
  Train *train;
  train = NULL;

  if (track->lost_trains.size > 0) {
    train = TrackAttemptToLocateTrain(track, se->id, ts);
  }
}

static void TrackHandleTrainCmd(Track *track, int train_num, int cmd) {
  Train t;
  if (track->train[train_num].status == TR_UNINIT) {
    t.num = train_num;
    TrackAddTrain(track, &t);
  }
}

// If we just have an RE, then we just pass the RE through
static void TrackHandleRawEvent(Track *track, RawEvent *re) {
  RawSensorEvent *se_event;
  RawSwitchEvent *sw_event;
  RawTrainCmdEvent *tr_cmd_event;

  switch (re->type) {
    case RE_SE:
      se_event = &re->event.se_event;
      TrackHandleTrainAtSensorRaw(track, se_event, re->timestamp);
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
    default:
      assert(0);
  }
}

static void TrackHandleTrainAtSensor(Track *track, EventGroup *grp) {
  int r, train_num, ekey, vts, rts, delta, exp;
  Train *train;
  track_node *new_pos;

  vts = grp->ve.timestamp;
  rts = grp->re.timestamp;
  delta = rts - vts;

  ekey = grp->ve.key;

  train_num = grp->ve.event.train_at.train_num;
  assert(IS_VALID_TR_NUM(train_num));
  train = &track->train[train_num];

  // if we get an unexpected key (from a past window) then:
  //   1) we got a false positive from a sensor
  //   2) something really weird is happening
  exp = ev_window_is_unexpected(&train->window, ekey);
  // assertf(exp == 0 || exp == 1, "%d\n", exp);
  if (exp) {
    train->pos = train->prev_pos;
    TrackLoseTrain(track, train);
    r = ev_window_remove_key(&train->window, ekey);
    assert(r == 0);
    ev_window_shift_all(&train->window);
    return;
  }

  r = ev_window_remove_key(&train->window, ekey);
  // assertf(r == 0, "%d %d\r\n", ekey, r);

  new_pos = &track->graph[grp->re.event.se_event.id];
  if (train->status == TR_KNOWN) {
    TrackUpdateKnownTrain(track, train, new_pos, rts);
  }
  else if (train->status == TR_UN_SPEED) {
    TrackUpdateUnknownSpeedTrain(track, train, new_pos, rts);
  }
  else {
    assert(0);
  }
}


// We didn't get a raw event, so we lose the train for now
static void TrackHandleTrainAtTimeout(Track *track, EventGroup *grp) {
  int esize, r, train_num, ekey, exp;
  Train *train;

  ekey = grp->ve.key;

  train_num = grp->ve.event.train_at.train_num;
  assert(IS_VALID_TR_NUM(train_num));
  train = &track->train[train_num];

  exp = ev_window_is_unexpected(&train->window, ekey);
  // assertf(exp == 0 || exp == 1, "%d\n", exp);

  if (exp) {
    // remove the key and ignore this event
    r = ev_window_remove_key(&train->window, ekey);
    assert(r == 0);
    return;
  }

  // we now have an expected event that has timed out

  // Here we have a potential problem:

  // if the timeout occurs with more than one item in the expected window
  // then we just remove the event key from the window
  esize = train->window.size;
  if (esize > 1) {
    r = ev_window_remove_key(&train->window, ekey);
    assert(r == 0);
  }
  // else if the timeout occurs with only one item in the expected window
  // then we have lost track of where the train is
  else if (esize == 1) {
    TrackLoseTrain(track, train);
    r = ev_window_remove_key(&train->window, ekey);
    assert(r == 0);
    ev_window_shift_all(&train->window);
  }
  else {
    // this should never happen
    assert(0);
  }
}


int TrackInterpretEventGroup(Track *track, EventGroup *grp) {
  switch (grp->type) {
    case VRE_VE_RE:
    case VRE_RE:
      TrackHandleTrainAtSensor(track, grp);
      TrackHandleRawEvent(track, &grp->re);
      break;
    case VRE_VE:
      TrackHandleTrainAtTimeout(track, grp);
      break;
    case RE:
      TrackHandleRawEvent(track, &grp->re);
      break;
    default:
      assert(0);
      break;
  }
  return 0;
}

