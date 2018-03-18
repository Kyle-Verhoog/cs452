#include <user/train/waiting_room.h>


void SendSensorDelta(tid_t wr_tid, tid_t cs_tid, tid_t my_tid, char *new_sensors, char *old_sensors) {
  int i, j, r;
  RawEvent event;
  char new_dec;
  char old_dec;

  event.type = RE_SW;
  event.timestamp = Time(cs_tid, my_tid);

  // looping over decoders A->E
  for (i = 0; i < DECODER_SIZE*2; ++i) {
    new_dec = new_sensors[i];
    old_dec = old_sensors[i];

    if (new_dec == old_dec) continue;

    // looping over bits of decoder byte
    for (j = 0; j < 8; ++j) {
      if (GET_SENSOR(new_dec, j) != GET_SENSOR(old_dec, j)) {
        event.event.se_event.id = i*8 + j;
        event.event.se_event.state = GET_SENSOR(new_dec, j);
        Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
      }
    }
  }
}


static void SensorSubscriber() {
  int r;
  SNSubscribe sn_sub;
  Sensors event, prev;
  tid_t sn_pub, wr_tid, cs_tid, my_tid;

  sn_pub = WhoIs(SENSOR_PUBLISHER_ID);
  assert(sn_pub > 0);

  wr_tid = MyParentTid();
  assert(wr_tid > 0);

  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid > 0);

  my_tid = MyTid();
  assert(my_tid > 0);  

  sn_sub.snr = SN_SUBSCRIBE;

  while (true) {
    Send(sn_pub, &sn_sub, sizeof(sn_sub), &event, sizeof(event));
    SendSensorDelta(wr_tid, cs_tid, my_tid, &event, &prev);
    memcpy(&prev, &event, sizeof(event));
  }

  Exit();
}


static void SwitchSubscriber() {
  int r;
  SWSubscribe sw_sub;
  RawEvent event;
  tid_t sw_pub, wr_tid, cs_tid;

  sw_pub = WhoIs(SWITCH_PUBLISHER_ID);
  assert(sw_pub > 0);

  wr_tid = MyParentTid();
  assert(wr_tid > 0);

  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid > 0);

  my_tid = MyTid();
  assert(my_tid > 0);

  sw_sub.swr = SW_SUBSCRIBE;
  event.type = RE_SW;
  event.timestamp = Time(cs_tid, my_tid);

  while (true) {
    Send(sw_pub, &sw_sub, sizeof(sw_sub), &event.event.sw_event, sizeof(RawSwitchEvent));
    Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void TrainSubscriber() {
  int r;
  TSubscribe tsub;
  RawEvent event;
  tid_t tr_pub, wr_tid, cs_tid;

  tr_pub = WhoIs(TRAIN_PUBLISHER_ID);
  assert(tr_pub > 0);

  wr_tid = MyParentTid();
  assert(wr_tid > 0);

  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid > 0);

  my_tid = MyTid();
  assert(my_tid > 0);

  tsub.tc = T_SUBSCRIBE;
  event.type = RE_TR_CMD;
  event.timestamp = Time(cs_tid, my_tid);

  while (true) {
    Send(tr_pub, &tsub, sizeof(tsub), &event.event.tr_cmd_event, sizeof(RawTrainCmdEvent));
    Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}


