#include <user/train/interpreter.h>

static tid_t tm_tid;
static void InterpretSensorEvent(RawSensorEvent *se_event, Track *track, TrackUpdate *update) {
  char *new_sensors, *old_sensors, new_dec, old_dec;
  int i, j;
  TrackEvent event;

  event.type = TE_SE_CHANGE;
  new_sensors = se_event->sensors;
  old_sensors = track->sensors;

  // check new sensors for deltas with the old sensor data
  // looping over decoders A->E
  for (i = 0; i < DECODER_SIZE*2; ++i) {
    new_dec = new_sensors[i];
    old_dec = old_sensors[i];

    if (new_dec == old_dec) continue;

    // looping over bits of decoder byte
    for (j = 0; j < 8; ++j) {
      if (GET_SENSOR(new_dec, j) != GET_SENSOR(old_dec, j)) {
        event.event.se_change.dec = i;
        event.event.se_change.sen = j;
        update->events[update->num++] = event;
      }
    }
  }

  // TODO: using updated sensors compare to predicted model...?
}

static void InterpretSwitchEvent(RawSwitchEvent *sw_event, Track *track, TrackUpdate *update) {
  TrackEvent event;
  event.type = TE_SW_CHANGE;
  event.event.sw_change.num = sw_event->sw;
  event.event.sw_change.newdir = sw_event->dir;
  update->events[update->num++] = event;
}

static void InterpretTrainCmdEvent(RawTrainCmdEvent *cmd_event, Track *track, TrackUpdate *update) {
  // TODO:
  // TETRGear event;
  // event.num = cmd_event->arg1;
  // event.newgear = cmd_event->arg2;
}

static void Interpret(tid_t rep_tid, RawEvent *rte) {
  int r;
  TrackRequest rep_req;
  Track track;
  TrackUpdate update;

  update.num = 0;

  // Request for the previous track state
  rep_req.type = TRR_FETCH;
  Send(rep_tid, &rep_req, sizeof(rep_req), &track, sizeof(track));

  switch (rte->type) {
    case RE_SE:
      InterpretSensorEvent(&rte->event.se_event, &track, &update);
      break;
    case RE_TR_CMD:
      InterpretTrainCmdEvent(&rte->event.tr_cmd_event, &track, &update);
      break;
    case RE_SW:
      InterpretSwitchEvent(&rte->event.sw_event, &track, &update);
      break;
    default:
      assert(0);
      break;
  }

  // Send the updated track state to the representer
  rep_req.type = TRR_UPDATE;
  rep_req.data.update = update;
  Send(rep_tid, &rep_req, sizeof(rep_req), &r, sizeof(r));
}

static void SensorSubscriber() {
  int r;
  SNSubscribe sn_sub;
  RawEvent event;
  tid_t sn_pub, inter_tid;

  sn_pub = WhoIs(SENSOR_PUBLISHER_ID);
  assert(sn_pub > 0);

  inter_tid = MyParentTid();
  assert(inter_tid > 0);

  sn_sub.snr = SN_SUBSCRIBE;
  event.type = RE_SE;

  while (true) {
    Send(sn_pub, &sn_sub, sizeof(sn_sub), &event.event.se_event, sizeof(RawSensorEvent));
    Send(inter_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void SwitchSubscriber() {
  int r;
  SWSubscribe sw_sub;
  RawEvent event;
  tid_t sw_pub, inter_tid;

  sw_pub = WhoIs(SWITCH_PUBLISHER_ID);
  assert(sw_pub > 0);

  inter_tid = MyParentTid();
  assert(inter_tid > 0);

  sw_sub.swr = SW_SUBSCRIBE;
  event.type = RE_SW;

  while (true) {
    Send(sw_pub, &sw_sub, sizeof(sw_sub), &event.event.sw_event, sizeof(RawSwitchEvent));
    Send(inter_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void TrainSubscriber() {
  int r;
  TSubscribe tsub;
  RawEvent event;
  tid_t tr_pub, inter_tid;

  tr_pub = WhoIs(TRAIN_PUBLISHER_ID);
  assert(tr_pub > 0);

  inter_tid = MyParentTid();
  assert(inter_tid > 0);

  tsub.tc = T_SUBSCRIBE;
  event.type = RE_TR_CMD;

  while (true) {
    Send(tr_pub, &tsub, sizeof(tsub), &event.event.tr_cmd_event, sizeof(RawTrainCmdEvent));
    Send(inter_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}


static void WaitingRoomCourier() {
  int r;
  tid_t int_tid, wr_tid;
  WRRequest wrr;
  EventGroup events;

  int_tid = MyParentTid();
  assert(int_tid > 0);

  wr_tid = WhoIs(WAITING_ROOM_ID);
  assert(wr_tid > 0);

  wrr.type = WR_CE;
  while (true) {
    Send(wr_tid, &wrr, sizeof(wrr), &events, sizeof(events));
    Send(int_tid, &events, sizeof(events), &r, sizeof(r));
  }
}

void Interpreter() {
  int r;
  tid_t rep_tid, req_tid;

  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);

  // Subscribers to data publishers
  Create(25, &WaitingRoomCourier);

  RawEvent rte;
  while (true) {
    Receive(&req_tid, &rte, sizeof(rte));
    Interpret(rep_tid, &rte);
    Reply(req_tid, &r, sizeof(r));
  }

  Exit();
}
