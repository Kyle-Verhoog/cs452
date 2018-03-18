#include <user/train/waiting_room.h>


void SendSensorDelta(tid_t wr_tid, tid_t cs_tid, tid_t my_tid, char *new_sensors, char *old_sensors) {
  int i, j, r;
  WRRequest event;
  char new_dec;
  char old_dec;

  event.type = WR_RE;
  event.data.re.type = RE_SW;
  event.data.re.timestamp = Time(cs_tid, my_tid);

  // looping over decoders A->E
  for (i = 0; i < DECODER_SIZE*2; ++i) {
    new_dec = new_sensors[i];
    old_dec = old_sensors[i];

    if (new_dec == old_dec) continue;

    // looping over bits of decoder byte
    for (j = 0; j < 8; ++j) {
      if (GET_SENSOR(new_dec, j) != GET_SENSOR(old_dec, j)) {
        event.data.re.event.se_event.id = i*8 + j;
        event.data.re.event.se_event.state = GET_SENSOR(new_dec, j);
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

  //TODO: INITIALIZE SENSORS
  init_sensors(&event);
  init_sensors(&prev);

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
  WRRequest event;
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
  event.type = WR_RE;
  event.data.re.type = RE_SW;

  while (true) {
    Send(sw_pub, &sw_sub, sizeof(sw_sub), &event.event.sw_event, sizeof(RawSwitchEvent));
    event.data.re.timestamp = Time(cs_tid, my_tid);
    Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void TrainSubscriber() {
  int r;
  TSubscribe tsub;
  WRRequest event;
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
  event.type = WR_RE;
  event.data.re.type = RE_TR_CMD;
  event.data.re.timestamp = Time(cs_tid, my_tid);

  while (true) {
    Send(tr_pub, &tsub, sizeof(tsub), &event.event.tr_cmd_event, sizeof(RawTrainCmdEvent));
    event.data.re.timestamp = Time(cs_tid, my_tid);
    Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

// ------------- WAITING ROOM SERVER ---------------- //

void TimeoutWR_VE(void *args){
  WRRequest wrr;
  int r;

  tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
  tid_t my_tid = MyTid();
  tid_t wr_tid = MyParentTid();
  assert(cs_tid > 0 && my_tid > 0 && wr_tid > 0);

  wrr.type = WR_TO;
  wrr.data = *(VirtualEvent *) args;

  Delay(cs_tid, my_tid, ve.timeout);

  Send(wr_tid, &wrr, sizeof(wrr), &r, sizeof(r));
  Exit();
}

void HandleWR_VE(WRRequest *event, VirtualEvent *waiting, int *sensorToVE){
  if(event->data.ve.type == VE_TR_AT){
    if(waiting[event->data.ve.key] != VE_REG){
      return;
    }else{
      //Spawn a timeout handler  
      CreateArgs(19, &TimeoutWR_VE, (void *)&waiting[event->data.ve.key], sizeof(VirtualEvent)); //TODO: UPDATE PRIORITY
    }
  }

  waiting[event->data.ve.key] = event->data.ve;
  sensorToVE[event->data.ve.event.train_at.node->num] = event->data.ve.key;
}

void HandleWR_RE(WRRequest *event, VirtualEvent *waiting, int *sensorToVE, tid_t courier, tid_t cs_tid, tid_t my_tid){
  EventGroup eg;
  int sensor;

  if(event->data.re.type != RE_SE){
    eg.type = RE;
    eg.re = event.data.re;
    eg.re.timestamp = Time(cs_tid, my_tid);
  }
  else{
    sensor = event->data.re.event.se_event.id;
    if(sensorToVE[sensor] != -1){
      eg.type = waiting[sensorToVE[sensor]].type == VE_REG ? VRE_RE : VRE_VE_RE;
      eg.re = event->data.re;
      eg.re.timestamp = Time(cs_tid, my_tid);
      eg.ve = waiting[sensorToVE[sensor]];
      sensorToVE[sensor] = -1;
      waiting[sensorToVE[sensor]].type = VE_NONE;
    }
    else{
      eg.type = RE;
      eg.re = event->data.re;
      eg.re.timestamp = Time(cs_tid, my_tid);
    }
  }

  courier = -1;
  Reply(courier, &eg, sizeof(eg));
}

void HandleWR_TO(WRRequest *event, VirtualEvent *waiting, int *sensorToVE, tid_t courier){
  EventGroup eg;
  int sensor;

  eg.type = VRE_VE;
  eg.ve = event.data.ve; 

  sensor = event->data.re.event.se_event.id;
  sensorToVE[sensor] = -1;
  waiting[sensorToVE[sensor]].type = VE_NONE;
}

void init_waiting_room(int *map){
  int i;
  for(i = 0; i < SENSOR_SIZE; i++){
    map[i] = -1;
  }
}

void WaitingRoom(){
  int r;
  WRRequest event;
  tid_t req_tid;
  tid_t courier = -1;

  VirtualEvent waiting[WAITING_ROOM_SIZE];
  int sensorToVE[SENSOR_SIZE];
  init_waiting_room(sensorToVE);

  r = RegisterAs(WAITING_ROOM_ID);
  assert(r == 0);
  tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
  tid_t my_tid = MyTid();

  Create(29, &TrainSubscriber);
  Create(29, &SwitchSubscriber);
  Create(29, &SensorSubscriber);

  while(true){

    Receive(&req_tid, &event, sizeof(event));    

    switch(event.type){
      case WR_VE:
        HANDLEWR_VE(&event, rve, sensorToVE);
        Reply(req_tid, &r, sizeof(r));
        break;
      case WR_RE:
        //TODO: ASSERT COURIER DOESN'T EXIST
        HandleWR_RE(&event, rve, sensorToVE, courier);
        Reply(req_tid, &r, sizeof(r));
        break;
      case WR_TO:
        HandleWR_TO(&event, rve, sensorToVE);
        Reply(req_tid, &r, sizeof(r)); 
      case WR_CE:
        courier = req_tid;
        break;
      default:
        assert(0 && "bad request");
    }
  }

  Exit();
}
