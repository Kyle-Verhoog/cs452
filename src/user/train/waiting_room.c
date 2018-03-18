
#include <user/train/waiting_room.h>


void SensorDelta() {

  /*
   *
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
  */
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


