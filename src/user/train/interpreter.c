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
  tid_t sw_prov, se_prov, tr_prov, rep_tid, req_tid;

  rep_tid = WhoIs(REPRESENTER_ID);

  // Subscribe to data providers
  // sw_prov = WhoIs(SWITCH_PROVIDER_ID);
  // se_prov = WhoIs(SENSOR_PROVIDER_ID);
  // tr_prov = WhoIs(TRAIN_PROVIDER_ID);

  RawTrackEvent rte;
  while (true) {
    Receive(&req_tid, &rte, sizeof(rte));
    Interpret(rep_tid, &rte);
    Reply(req_tid, &r, sizeof(r));
  }
}
