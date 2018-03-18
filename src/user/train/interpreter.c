#include <user/train/interpreter.h>

CIRCULAR_BUFFER_DEF(VEList, VirtualEvent, VIRTUAL_EVENT_LIST_SIZE);


static tid_t tm_tid;
static void UpdateSensorEvent(RawSensorEvent *se_event, Track *track, TrackUpdate *update) {
  // TODO once new se_event defined
  TrackEvent event;
  event.type = TE_SE_CHANGE;

  event.event.se_change.dec = se_event->id/16;
  event.event.se_change.sen = se_event->id%16;
  update->events[update->num++] = event;
}


static void UpdateSwitchEvent(RawSwitchEvent *sw_event, Track *track, TrackUpdate *update) {
  TrackEvent event;
  event.type = TE_SW_CHANGE;
  event.event.sw_change.num = sw_event->sw;
  event.event.sw_change.newdir = sw_event->dir;
  update->events[update->num++] = event;
}

// TODO: look in both directions of the node for other possible trains
static void AttemptToAdoptTrain(RawSensorEvent *re, Track *track, TrackUpdate *update) {
  int r;
  Train *t;

  r = train_list_pop(&track->orphan_trains, &t);
  assert(r == 0);
  TMLogStrf(tm_tid, "adopting train %d at %s\n", t->num, t->pos->name);

  t->pos = &TRACK[re->id];
  VirtualEvent ve;
  ve.type = VE_TR_AT;
  ve.timestamp = 2000;
  ve.timeout = 2000;
  ve.depend = 38;
  ve.event.train_at.train_num = t->num;
  ve.event.train_at.node = t->pos;

  r = train_list_push(&track->active_trains, t);
  assert(r == 0);
}

static void UpdateTrainCmdEvent(RawTrainCmdEvent *cmd_event, Track *track, TrackUpdate *update) {
  // TODO:
  // TETRGear event;
  // event.num = cmd_event->arg1;
  // event.newgear = cmd_event->arg2;
}

#define alpha 90
// recalculate a rough estimate of velocity
static void UpdateTrain(Train *train, track_node *new_pos, Track *track, VEList *events, int ts) {
  int dist, i, r, old_speed, new_speed;
  track_node *old_pos, *sensor;
  poss_node_list pnl;

  TMLogStrf(tm_tid, "updating train %d\n", train->num);
  poss_node_list_init(&pnl);

  old_speed = train->speed;
  assert(old_speed >= 0 && train->speed <= 10000);

  old_pos = train->pos;
  assert(old_pos != NULL);
  dist = track_node_dist(old_pos, new_pos); // dist is in mm
  assert(dist >= 0 && dist <= 10000);

  new_speed = (dist*1000) / ((ts - train->timestamp)*10); // speed in um/ms

  train->timestamp = ts;
  train->speed = (alpha*new_speed + (100-alpha)*train->speed) / 100;
  train->pos = new_pos;

  r = TrackGetNextPossibleSensors(track, new_pos, &pnl);
  assert(pnl.size > 0);
  VirtualEvent ve;
  ve.type = VE_TR_AT;
  for (i = 0; i < pnl.size; ++i) {
    r = poss_node_list_pop(&pnl, &sensor);
    assert(r == 0);
    dist = track_node_dist(new_pos, sensor);
    ve.timeout = (dist*1000) / (train->speed*10);
    ve.depend = sensor->num;
    ve.event.train_at.train_num = train->num;
    ve.event.train_at.node = sensor;
    VEList_push(events, ve);
  }
}

static void InterpretVREVERE(EventGroup *grp, Track *track, TrackUpdate *update, VEList *vevents) {
  TMLogStrf(tm_tid, "VRE VE RE\n");
  int vts, rts, delta;
  Train *train;
  track_node *new_pos;

  vts = grp->ve.timestamp;
  rts = grp->re.timestamp;
  assert(vts <= rts);

  delta = rts - vts;

  assert(IS_VALID_TR_NUM(grp->ve.event.train_at.train_num));
  train = GetActiveTrain(track, grp->ve.event.train_at.train_num);

  new_pos = &TRACK[grp->re.event.se_event.id];

  UpdateTrain(train, new_pos, track, vevents, rts);
}

static void InterpretVRERE(EventGroup *grp, Track *track, TrackUpdate *update, VEList *vevents) {
  TMLogStrf(tm_tid, "VRE RE\n");
  int vts, rts, delta;
  Train *train;
  track_node *new_pos;

  vts = grp->ve.timestamp;
  rts = grp->re.timestamp;
  assert(vts >= rts);

  delta = rts - vts;

  assert(IS_VALID_TR_NUM(grp->ve.event.train_at.train_num));
  train = GetActiveTrain(track, grp->ve.event.train_at.train_num);

  new_pos = &TRACK[grp->re.event.se_event.id];

  UpdateTrain(train, new_pos, track, vevents, rts);
}

// We didn't get a raw event, so we orphan the train for now
static void InterpretVREVE(EventGroup *grp, Track *track, TrackUpdate *update, VEList *vevents) {
  TMLogStrf(tm_tid, "VRE VE\n");
  int r, tr_num;
  Train *train;

  tr_num = grp->ve.event.train_at.train_num;
  assert(IS_VALID_TR_NUM(tr_num));

  // TODO: replace with a list datastructure

  train = RemoveActiveTrain(track, tr_num);
  assert(train != 0);

  r = train_list_push(&track->orphan_trains, train);
  assert(r == 0);
}

// If we just have an RE, then we just pass the RE through
static void InterpretRE(RawEvent *re, Track *track, TrackUpdate *update) {
  switch (re->type) {
    case RE_SE:
      TMLogStrf(tm_tid, "RE\n");
      // if there is an orphaned train, see if it makes sense to update its position
      // to this sensor
      if (track->orphan_trains.size > 0) {
        AttemptToAdoptTrain(&re->event.se_event, track, update);
      }
      UpdateSensorEvent(&re->event.se_event, track, update);
      break;
    case RE_SW:
      UpdateSwitchEvent(&re->event.sw_event, track, update);
      break;
    case RE_TR_CMD:
      UpdateTrainCmdEvent(&re->event.tr_cmd_event, track, update);
      break;
    default:
      assert(0);
  }
}

static void SendVirtualEvents(tid_t vep_tid, VEList *events) {
  int r;
  VERequest ver;
  VirtualEvent ve;

  ver.type = VER_EVENT;

  while (events->size > 0) {
    r = VEList_pop(events, &ve);
    assert(r == 0);
assert(0);

    ver.ve = ve;
    Send(vep_tid, &ver, sizeof(ver), &r, sizeof(r));
  }
}

static void Interpret(tid_t rep_tid, tid_t vep_tid, Track *track, EventGroup *grp) {
  int r;
  TrackRequest rep_req;
  TrackUpdate update;
  VEList vevents;

  update.num = 0;
  VEList_init(&vevents);

  // TODO?: pull in speed models

  switch (grp->type) {
    case VRE_VE_RE:
      assert(grp->type == RE_SE);
      InterpretVREVERE(grp, track, &update, &vevents);
      break;
    case VRE_VE:
      assert(grp->type == RE_SE);
      InterpretVREVE(grp, track, &update, &vevents);
      break;
    case VRE_RE:
      assert(grp->type == RE_SE);
      InterpretVRERE(grp, track, &update, &vevents);
      break;
    case RE:
      InterpretRE(&grp->re, track, &update);
      break;
    default:
      assert(0);
      break;
  }

  SendVirtualEvents(vep_tid, &vevents);

  // Send the updates to the track state to the representer
  rep_req.type = TRR_UPDATE;
  rep_req.data.update = update;
  Send(rep_tid, &rep_req, sizeof(rep_req), &r, sizeof(r));
}

static void WaitingRoomCourier() {
  int r;
  tid_t int_tid, wr_tid;
  WRRequest wrr;
  EventGroup group;

  int_tid = MyParentTid();
  assert(int_tid > 0);

  wr_tid = WhoIs(WAITING_ROOM_ID);
  assert(wr_tid > 0);

  wrr.type = WR_CE;
  while (true) {
    Send(wr_tid, &wrr, sizeof(wrr), &group, sizeof(group));
    Send(int_tid, &group, sizeof(group), &r, sizeof(r));
  }
}

void Interpreter() {
  int r;
  tid_t rep_tid, req_tid, vep_tid;
  Track track;

  TrackInit(&track);

  rep_tid = WhoIs(REPRESENTER_ID);
  vep_tid = WhoIs(VIRTUAL_PROVIDER_ID);
  tm_tid  = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0 && rep_tid > 0 && vep_tid > 0);

  // Subscribers to data publishers
  Create(26, &WaitingRoomCourier);

  EventGroup group;
  while (true) {
    Receive(&req_tid, &group, sizeof(group));
    Interpret(rep_tid, vep_tid, &track, &group);
    Reply(req_tid, &r, sizeof(r));
  }

  Exit();
}


