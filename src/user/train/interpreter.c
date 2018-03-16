#include <user/train/interpreter.h>

static void InterpretSensorEvent(RawSensorEvent *se_event, Track *track, TrackUpdate *update) {
  // TODO
}

static void InterpretSwitchEvent(RawSwitchEvent *sw_event, Track *track, TrackUpdate *update) {
  // TODO
}

static void InterpretTrainEvent(RawTrainEvent *tr_event, Track *track, TrackUpdate *update) {
  // update->events[0] = 
  // TODO
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
    case RTE_SENSOR:
      InterpretSensorEvent(&rte->event.se_event, &track, &update);
      break;
    case RTE_TRAIN:
      InterpretTrainEvent(&rte->event.tr_event, &track, &update);
      break;
    case RTE_SWITCH:
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
  tid_t se_pub;
  se_pub = WhoIs(SENSOR_PUBLISHER_ID);
  assert(se_pub > 0);

  // TODO

  Exit();
}

static void SwitchSubscriber() {
  tid_t sw_pub;
  sw_pub = WhoIs(SWITCH_PUBLISHER_ID);
  assert(sw_pub > 0);

  // TODO

  Exit();
}

static void TrainSubscriber() {
  int r;
  TMSubscribe tmsub;
  RawTrainEvent event;
  tid_t tr_pub, inter_tid;

  tr_pub = WhoIs(TRAIN_PUBLISHER_ID);
  assert(tr_pub > 0);

  inter_tid = MyParentTid();
  assert(inter_tid > 0);

  tmsub.tmc = TM_SUBSCRIBE;

  while (true) {
    Send(tr_pub, &tmsub, sizeof(tmsub), &event, sizeof(event));
    Send(inter_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

void Interpreter() {
  int r;
  tid_t rep_tid, req_tid;

  rep_tid = WhoIs(REPRESENTER_ID);

  // Subscribers to data publishers
  Create(25, &TrainSubscriber);
  // TODO: Create(25, &SensorSubscriber);
  // TODO: Create(25, &SwitchSubscriber);

  RawTrackEvent rte;
  while (true) {
    Receive(&req_tid, &rte, sizeof(rte));
    Interpret(rep_tid, &rte);
    Reply(req_tid, &r, sizeof(r));
  }

  Exit();
}
