#include <user/train/interpreter.h>

tid_t tm_tid;

static void InterpretSensorEvent(RawSensorEvent *se_event, Track *track, TrackUpdate *update) {
  // TODO
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

static void Interpret(tid_t rep_tid, RawTrackEvent *rte) {
  int r;
  TrackRequest rep_req;
  Track track;
  TrackUpdate update;

  update.num = 0;

  // Request for the previous track state
  rep_req.type = TRR_FETCH;
  Send(rep_tid, &rep_req, sizeof(rep_req), &track, sizeof(track));

  switch (rte->type) {
    case RTE_SE:
      InterpretSensorEvent(&rte->event.se_event, &track, &update);
      break;
    case RTE_TR_CMD:
      InterpretTrainCmdEvent(&rte->event.tr_cmd_event, &track, &update);
      break;
    case RTE_SW:
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
  RawTrackEvent event;
  tid_t sn_pub, inter_tid;

  sn_pub = WhoIs(SENSOR_PUBLISHER_ID);
  assert(sn_pub > 0);

  inter_tid = MyParentTid();
  assert(inter_tid > 0);

  sn_sub.snr = SN_SUBSCRIBE;
  event.type = RTE_SE;

  while (true) {
    Send(sn_pub, &sn_sub, sizeof(sn_sub), &event.event.se_event, sizeof(RawSensorEvent));
    Send(inter_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void SwitchSubscriber() {
  int r;
  SWSubscribe sw_sub;
  RawTrackEvent event;
  tid_t sw_pub, inter_tid;

  sw_pub = WhoIs(SWITCH_PUBLISHER_ID);
  assert(sw_pub > 0);

  inter_tid = MyParentTid();
  assert(inter_tid > 0);

  sw_sub.swr = SW_SUBSCRIBE;
  event.type = RTE_SW;

  while (true) {
    Send(sw_pub, &sw_sub, sizeof(sw_sub), &event.event.sw_event, sizeof(RawSwitchEvent));
    Send(inter_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void TrainSubscriber() {
  int r;
  TSubscribe tsub;
  RawTrackEvent event;
  tid_t tr_pub, inter_tid;

  tr_pub = WhoIs(TRAIN_PUBLISHER_ID);
  assert(tr_pub > 0);

  inter_tid = MyParentTid();
  assert(inter_tid > 0);

  tsub.tc = T_SUBSCRIBE;
  event.type = RTE_TR_CMD;

  while (true) {
    Send(tr_pub, &tsub, sizeof(tsub), &event.event.tr_cmd_event, sizeof(RawTrainCmdEvent));
    Send(inter_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

void Interpreter() {
  int r;
  tid_t rep_tid, req_tid;

  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);

  // Subscribers to data publishers
  Create(25, &TrainSubscriber);
  Create(25, &SensorSubscriber);
  Create(25, &SwitchSubscriber);

  RawTrackEvent rte;
  while (true) {
    Receive(&req_tid, &rte, sizeof(rte));
    Interpret(rep_tid, &rte);
    Reply(req_tid, &r, sizeof(r));
  }

  Exit();
}
