#include <test/train/track.h>

static track_node T[TRACK_MAX];

static void track_init_test(track_node *tr) {
  Track track;
  TrackInit(&track, tr);
}

static EventGroup MockSensor(int type, int rts, char *sname, VirtualEvent *ve) {
  EventGroup event;
  event.type = type;
  event.re.timestamp = rts;
  event.re.type = RE_SE;
  event.re.event.se_event.id = trhr(T, sname);
  event.re.event.se_event.state = 1;
  if (ve) {
    event.ve = *ve;
  }
  return event;
}

static void TrackClearEvents(Track *track) {
  update_list_init(&track->updates);
  ve_list_init(&track->vevents);
}

static void next_sensors_test(track_node *tr) {
  int dist;
  Track track;
  TrackInit(&track, tr);

  poss_node_list pnl;
  poss_node_list_init(&pnl);

  track_node *start;
  start = &tr[trhr(tr, "A1")];
  assert(start->type == NODE_SENSOR);
  dist = start->edge[DIR_AHEAD].dist;
  start = start->edge[DIR_AHEAD].dest;
  GetNextPossibleSensors(start, dist, &pnl);
  assert(pnl.size == 1);

  PossibleSensor sensor;
  poss_node_list_pop(&pnl, &sensor);
  assert(sensor.node->num == 44);
  assert(sensor.dist == 231+188+43);

  start = sensor.node->edge[DIR_AHEAD].dest;
  dist = sensor.node->edge[DIR_AHEAD].dist;
  GetNextPossibleSensors(start, dist, &pnl);
  assert(pnl.size == 1);
  poss_node_list_pop(&pnl, &sensor);
  assert(sensor.node->num == 70);
  assert(sensor.dist == 875);

  start = sensor.node->edge[DIR_AHEAD].dest;
  dist = sensor.node->edge[DIR_AHEAD].dist;
  GetNextPossibleSensors(start, dist, &pnl);
  assert(pnl.size == 1);
  poss_node_list_pop(&pnl, &sensor);
  assert(sensor.node->num == 54);
  assert(sensor.dist == 384);

  start = sensor.node->edge[DIR_AHEAD].dest;
  dist = sensor.node->edge[DIR_AHEAD].dist;
  GetNextPossibleSensors(start, dist, &pnl);
  assert(pnl.size == 2);
  poss_node_list_pop(&pnl, &sensor);
  assert(sensor.node->num == 56);
  assert(sensor.dist == 309+155+316);
  poss_node_list_pop(&pnl, &sensor);
  assert(sensor.node->num == 73);
  assert(sensor.dist == 309+155+239);

  start = tr[1].edge[DIR_AHEAD].dest;
  GetNextPossibleSensors(start, dist, &pnl);
  poss_node_list_pop(&pnl, &sensor);
  assert(pnl.size == 0);

  //start = &tr[70]
}


// A basic test of a train being picked up at A1 and being tracked to D7
static void basic_test(track_node *g) {
  Track track;
  Train train;
  EventGroup event;
  VirtualEvent ve;
  TrackInit(&track, g);

  // emulate finding train 24
  train.num = 24;
  TrackAddTrain(&track, &train);
  assert(track.train[24].num == 24);
  assert(track.train[24].status == TR_LOST);

  // simulate a sensor hit at A1
  event.type = RE;
  event.re.timestamp = 1000;
  event.re.type = RE_SE;
  event.re.event.se_event.id = 0; // A1
  event.re.event.se_event.state = 1;
  TrackInterpretEventGroup(&track, &event);
  // printf("%d\n", track.lost_trains.size);
  assert(track.lost_trains.size == 0);
  assert(track.active_trains.size == 1);
  assert(track.vevents.size == 1);
  update_list_init(&track.updates);
  ve_list_pop(&track.vevents, &ve);
  assert(ve.type == VE_TR_AT);
  assert(ve.key == 1);
  assert(ve.timeout == -1);
  assert(ve.timestamp == 1000+200);
  assert(track.train[24].num == 24);
  assert(track.train[24].sen_ts == 1000);
  assert(track.train[24].speed == -1);
  assert(ve.event.train_at.train_num == 24);
  assert(ve.event.train_at.node->num == 44);


  // simulate the next sensor hit at C13 with the VER
  event.type = VRE_RE;
  event.re.timestamp = 1100;
  event.re.type = RE_SE;
  event.re.event.se_event.id = 44; // C13
  event.re.event.se_event.state = 1;
  event.ve = ve;
  TrackInterpretEventGroup(&track, &event);
  assert(track.lost_trains.size == 0);
  assert(track.active_trains.size == 1);
  assert(track.train[24].sen_ts == 1100);
  assert(track.train[24].speed == 2310);
  assert(track.vevents.size == 1);
  ve_list_pop(&track.vevents, &ve);
  assert(ve.key == 2);
  assert(ve.type == VE_TR_AT);
  assert(ve.timeout == 166);
  assert(ve.timestamp == 1100+378);
  assert(ve.event.train_at.train_num == 24);
  assert(ve.event.train_at.node->num == 70);

  // simulate the next sensor hit at E7 with the VER AFTER the VE has come in
  // (model was too quick in prediction)
  event.type = VRE_VE_RE;
  event.re.timestamp = 1500;
  event.re.type = RE_SE;
  event.re.event.se_event.id = 70; // E7
  event.re.event.se_event.state = 1;
  event.ve = ve;
  assert(track.train[24].status == TR_KNOWN);
  TrackInterpretEventGroup(&track, &event);
  assert(track.lost_trains.size == 0);
  assert(track.active_trains.size == 1);
  assert(track.train[24].sen_ts == 1500);
  assert(track.train[24].speed == 2199);
  assert(track.vevents.size == 1);
  update_list_init(&track.updates);
  ve_list_pop(&track.vevents, &ve);
  assert(ve.type == VE_TR_AT);
  assert(ve.timeout == 140);
  assert(ve.timestamp == 1500+174);
  assert(ve.event.train_at.train_num == 24);
  // printf("%s\n", ve.event.train_at.node->name);
  assert(ve.event.train_at.node->num == 54);

  // simulate the next sensor hit at D7 with the VER AFTER the VE has come in
  // (model was too quick in prediction)
  event.type = VRE_RE;
  event.re.timestamp = 1600;
  event.re.type = RE_SE;
  event.re.event.se_event.id = 54; // D7
  event.re.event.se_event.state = 1;
  event.ve = ve;
  assert(track.train[24].status == TR_KNOWN);
  TrackInterpretEventGroup(&track, &event);
  assert(track.lost_trains.size == 0);
  assert(track.active_trains.size == 1);
  assert(track.train[24].sen_ts == 1600);
  // printf("%d\n", track.train[24].speed);
  assert(track.train[24].speed == 3675);
  assert(track.vevents.size == 2);
  update_list_init(&track.updates);
  ve_list_pop(&track.vevents, &ve);
  assert(ve.type == VE_TR_AT);
  // printf("%d\n", ve.timeout);
  assert(ve.timeout == 100);
  assert(ve.timestamp == 1600+212);
  assert(ve.event.train_at.train_num == 24);
  // printf("%s\n", ve.event.train_at.node->name);
  assert(ve.event.train_at.node->num == 56);
  update_list_init(&track.updates);
  ve_list_pop(&track.vevents, &ve);
  assert(ve.type == VE_TR_AT);
  // printf("%d\n", ve.timeout);
  assert(ve.timeout == 102);
  assert(ve.timestamp == 1600+191);
  assert(ve.event.train_at.train_num == 24);
  // printf("%s\n", ve.event.train_at.node->name);
  assert(ve.event.train_at.node->num == 73);
}


static void track_events_test() {
  Track track;
  Train train;
  EventGroup event;
  TrackEvent te;
  VirtualEvent ve;
  TrackInit(&track, T);

  train.num = 24;
  TrackAddTrain(&track, &train);
  assert(track.train[24].num == 24);
  assert(track.train[24].status == TR_LOST);
  assert(track.updates.size == 1);
  update_list_pop(&track.updates, &te);
  assert(te.type == TE_TR_STATUS);
  assert(te.event.tr_status.num == 24);
  assert(te.event.tr_status.old == TR_UNINIT);
  assert(te.event.tr_status.new == TR_LOST);

  event = MockSensor(RE, 1000, "A1", NULL);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  update_list_pop(&track.updates, &te);
  assert(te.type == TE_TR_STATUS);
  assert(te.event.tr_status.num == 24);
  assert(te.event.tr_status.old == TR_LOST);
  assert(te.event.tr_status.new == TR_UN_SPEED);
  update_list_pop(&track.updates, &te);
  assert(te.type == TE_TR_POS);
  assert(te.event.tr_pos.num == 24);
  assert(te.event.tr_pos.node == &T[trhr(T, "A1")]);
  update_list_pop(&track.updates, &te);
  assert(te.type == TE_SE_CHANGE);
  assert(te.event.se_change.num == trhr(T, "A1"));
  assert(te.event.se_change.state == 1);
  assert(track.updates.size == 0);

  event = MockSensor(VRE_RE, 1500, "C13", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  assert(ve.type == VE_TR_AT);
  ve_list_init(&track.vevents);
  update_list_pop(&track.updates, &te);
  assert(te.type == TE_TR_STATUS);
  assert(te.event.tr_status.num == 24);
  assert(te.event.tr_status.old == TR_UN_SPEED);
  assert(te.event.tr_status.new == TR_KNOWN);

  update_list_pop(&track.updates, &te);
  assert(te.type == TE_TR_SPEED);
  assert(te.event.tr_speed.num == 24);
  assert(te.event.tr_speed.old == -1);
  assert(te.event.tr_speed.new == 462);

  update_list_pop(&track.updates, &te);
  assert(te.type == TE_TR_POS);
  assert(te.event.tr_pos.num == 24);
  assert(te.event.tr_pos.node == &T[trhr(T, "C13")]);

  update_list_pop(&track.updates, &te);
  assert(te.type == TE_SE_CHANGE);
  assert(te.event.se_change.num == trhr(T, "C13"));

  assert(track.updates.size == 0);
}


static void lost_train_test() {
  Track track;
  Train train;
  EventGroup event;
  TrackEvent te;
  VirtualEvent ve;
  TrackInit(&track, T);

  train.num = 24;
  TrackAddTrain(&track, &train);

  event = MockSensor(RE, 1000, "A1", NULL);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  event = MockSensor(VRE_RE, 1500, "C13", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  assert(track.train[24].status == TR_KNOWN);
  event = MockSensor(VRE_VE, -1, "C13", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  assert(track.train[24].status == TR_LOST);

  event = MockSensor(RE, 1700, "E7", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  assert(track.train[24].status == TR_UN_SPEED);

  event = MockSensor(VRE_RE, 1800, "D7", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  assert(track.train[24].status == TR_KNOWN);
}

// test a sensor being activated nowhere near the lost train
static void lost_train_false_positive_test() {
  Track track;
  Train train;
  EventGroup event;
  TrackEvent te;
  VirtualEvent ve;
  TrackInit(&track, T);

  train.num = 24;
  TrackAddTrain(&track, &train);

  event = MockSensor(RE, 1000, "A1", NULL);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  event = MockSensor(VRE_RE, 1500, "C13", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  assert(track.train[24].status == TR_KNOWN);
  event = MockSensor(VRE_VE, -1, "C13", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  assert(track.train[24].status == TR_LOST);

  event = MockSensor(RE, 1700, "E13", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);

  assert(track.train[24].status == TR_LOST);
}

static void two_trains_test() {
  Track track;
  Train train;
  EventGroup event;
  VirtualEvent ve;
  TrackInit(&track, T);

  // emulate finding train 24
  train.num = 24;
  TrackAddTrain(&track, &train);

  event = MockSensor(RE, 1000, "A1", NULL);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  ve_list_init(&track.vevents);
  update_list_init(&track.updates);

  event = MockSensor(VRE_RE, 1500, "C13", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  ve_list_init(&track.vevents);
  update_list_init(&track.updates);
  assert(track.train[24].status == TR_KNOWN);

  event = MockSensor(VRE_RE, 2000, "E7", &ve);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  ve_list_init(&track.vevents);
  update_list_init(&track.updates);
  assert(track.train[24].status == TR_KNOWN);


  // emulate finding train 74
  train.num = 74;
  TrackAddTrain(&track, &train);

  event = MockSensor(RE, 1000, "A1", NULL);
  TrackInterpretEventGroup(&track, &event);
  ve_list_pop(&track.vevents, &ve);
  TrackClearEvents(&track);
  ve_list_init(&track.vevents);
  update_list_init(&track.updates);

  event = MockSensor(VRE_RE, 1200, "C13", &ve);
  TrackInterpretEventGroup(&track, &event);

  // check that both trains are known
  assert(track.train[24].status == TR_KNOWN);
  assert(track.train[74].status == TR_KNOWN);
}

void track_tests() {
  init_tracka(T);

  track_init_test(T);
  next_sensors_test(T);
  basic_test(T);
  track_events_test();
  lost_train_test();
  two_trains_test();
  lost_train_false_positive_test();
}
