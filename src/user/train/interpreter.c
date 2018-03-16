#include <user/train/interpreter.h>


void InterpretSensorEvent(RawSensorEvent *se_event, Track *track, TrackUpdate *updates) {
  // TODO
}

void InterpretSwitchEvent(RawSwitchEvent *sw_event, Track *track, TrackUpdate *updates) {
  // TODO
}

void InterpretTrainEvent(RawTrainEvent *tr_event, Track *track, TrackUpdate *updates) {
  // TODO
}


void Interpret(tid_t rep_tid, RawTrackEvent *rte) {
  int r;
  TrackRequest rep_req;
  Track track;
  TrackUpdate updates;

  updates.num = 0;

  // Request for the previous track state
  rep_req.type = TRR_FETCH;
  Send(rep_tid, &rep_req, sizeof(rep_req), &track, sizeof(track));


  switch (rte->type) {
    case RTE_SENSOR:
      InterpretSensorEvent(&rte->event.se_event, &track, &updates);
      break;
    case RTE_TRAIN:
      InterpretTrainEvent(&rte->event.tr_event, &track, &updates);
      break;
    case RTE_SWITCH:
      InterpretSwitchEvent(&rte->event.sw_event, &track, &updates);
      break;
    default:
      assert(0);
      break;
  }

  // Send the updated track state to the representer
  rep_req.type = TRR_UPDATE;
  rep_req.update = updates;
  Send(rep_tid, &rep_req, sizeof(rep_req), &r, sizeof(r));
}


void Interpreter() {
  int r;
  tid_t sw_pub, se_pub, tr_pub, rep_tid, req_tid;
  TMSubscribe tmsub;

  rep_tid = WhoIs(REPRESENTER_ID);

  // Subscribe to data providers
  sw_pub = WhoIs(SWITCH_PUBLISHER_ID);
  se_pub = WhoIs(SENSOR_PUBLISHER_ID);
  tr_pub = WhoIs(TRAIN_PUBLISHER_ID);

  tmsub.tmc = TM_SUBSCRIBE;
  Send(tr_pub, &tmsub, sizeof(tmsub), &r, sizeof(r));

  RawTrackEvent rte;
  while (true) {
    Receive(&req_tid, &rte, sizeof(rte));
    Interpret(rep_tid, &rte);
    Reply(req_tid, &r, sizeof(r));
  }
}
