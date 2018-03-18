#include <user/train/waiting_room.h>


void SendSensorDelta(tid_t wr_tid, tid_t cs_tid, tid_t my_tid, char *new_sensors, char *old_sensors) {
  int i, j, r;
  WRRequest event;
  char new_dec;
  char old_dec;

  event.type = WR_RE;
  event.data.re.type = RE_SE;
  event.data.re.timestamp = Time(cs_tid, my_tid);

  // looping over decoders A->E
  for (i = 0; i < DECODER_SIZE*2; ++i) {
    new_dec = new_sensors[i];
    old_dec = old_sensors[i];

    if (new_dec == old_dec) continue;

    // looping over bits of decoder byte
    for (j = 0; j < 8; ++j) {
      if (GET_SENSOR(new_dec, j) != GET_SENSOR(old_dec, j)) {
        event.data.re.event.se_event.id = i*8 + (7-j);
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

  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid > 0);

  wr_tid = MyParentTid();
  assert(wr_tid > 0);

  my_tid = MyTid();
  assert(my_tid > 0);  

  sn_sub.snr = SN_SUBSCRIBE;

  //TODO: INITIALIZE SENSORS
  init_sensors(&event);
  init_sensors(&prev);

  while (true) {
    Send(sn_pub, &sn_sub, sizeof(sn_sub), &event, sizeof(event));
    SendSensorDelta(wr_tid, cs_tid, my_tid, event.sensors, prev.sensors);
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

 // my_tid = MyTid();
 // assert(my_tid > 0);

  sw_sub.swr = SW_SUBSCRIBE;
  event.type = WR_RE;

  while (true) {
    Send(sw_pub, &sw_sub, sizeof(sw_sub), &event.data.re, sizeof(RawEvent));
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

  //my_tid = MyTid();
  //iassert(my_tid > 0);

  tsub.tc = T_SUBSCRIBE;
  event.type = WR_RE;

  while (true) {
    Send(tr_pub, &tsub, sizeof(tsub), &event.data.re, sizeof(RawEvent));
    Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void VirtualEventSubscriber(){
  int r;
  VESubscribe vsub;
  WRRequest event;
  tid_t ve_pub, wr_tid, cs_tid;

  ve_pub = WhoIs(VIRTUAL_PUBLISHER_ID);
  assert(ve_pub > 0);

  wr_tid = MyParentTid();
  assert(wr_tid > 0);

  //my_tid = MyTid();
  //assert(my_tid > 0);

  vsub.type = VER_SUBSCRIBE;
  event.type = WR_VE;
  
  while(true){
    Send(ve_pub, &vsub, sizeof(vsub), &event.data.ve, sizeof(VirtualEvent));
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
  wrr.data.ve = *(VirtualEvent *) args;

  Delay(cs_tid, my_tid, wrr.data.ve.timeout);

  Send(wr_tid, &wrr, sizeof(wrr), &r, sizeof(r));
  Exit();
}

void HandleWR_VE(WRRequest *event, VirtualEvent *waiting, int *sensorToVE){
  if(event->data.ve.type == VE_TR_AT){
    if(waiting[event->data.ve.key].type != VE_REG){
      return;
    }else{
      //Spawn a timeout handler  
      CreateArgs(19, &TimeoutWR_VE, (void *)&waiting[event->data.ve.key], sizeof(VirtualEvent)); //TODO: UPDATE PRIORITY
    }
  }

  waiting[event->data.ve.key] = event->data.ve;
  sensorToVE[event->data.ve.event.train_at.node->num] = event->data.ve.key;
}

void HandleWR_RE(WRRequest *event, VirtualEvent *waiting, int *sensorToVE, tid_t courier, tid_t my_tid){
  EventGroup eg;
  int sensor;

  if(event->data.re.type != RE_SE){
    eg.type = RE;
    eg.re = event->data.re;
  }
  else{
    sensor = event->data.re.event.se_event.id;
    if(sensorToVE[sensor] != -1){
      eg.type = waiting[sensorToVE[sensor]].type == VE_REG ? VRE_RE : VRE_VE_RE;
      eg.re = event->data.re;
      eg.ve = waiting[sensorToVE[sensor]];
      sensorToVE[sensor] = -1;
      waiting[sensorToVE[sensor]].type = VE_NONE;
    }
    else{
      eg.type = RE;
      eg.re = event->data.re;
    }
  }

  Reply(courier, &eg, sizeof(eg));
}

void HandleWR_TO(WRRequest *event, VirtualEvent *waiting, int *sensorToVE, tid_t courier){
  EventGroup eg;
  int sensor;

  eg.type = VRE_VE;
  eg.ve = event->data.ve; 

  sensor = event->data.re.event.se_event.id;
  sensorToVE[sensor] = -1;
  waiting[sensorToVE[sensor]].type = VE_NONE;
  Reply(courier, &eg, sizeof(eg));
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

  VirtualEvent waiting[KEY_SIZE];
  int sensorToVE[SENSOR_SIZE];
  init_waiting_room(sensorToVE);

  r = RegisterAs(WAITING_ROOM_ID);
  assert(r == 0);
  tid_t my_tid = MyTid();

  Create(26, &TrainProvider);
  Create(26, &SwitchProvider);
  Create(26, &SensorProvider);
  Create(26, &VirtualProvider);

  Create(26, &TrainSubscriber);
  Create(26, &SwitchSubscriber);
  Create(26, &SensorSubscriber);
  Create(26, &VirtualEventSubscriber);

  while(true){

    Receive(&req_tid, &event, sizeof(event));    

    switch(event.type){
      case WR_VE:
        HandleWR_VE(&event, waiting, sensorToVE);
        Reply(req_tid, &r, sizeof(r));
        break;
      case WR_RE:
        assert(courier != -1);
        HandleWR_RE(&event, waiting, sensorToVE, courier, my_tid);
        courier = -1;
        Reply(req_tid, &r, sizeof(r));
        break;
      case WR_TO:
        assert(courier != -1);
        HandleWR_TO(&event, waiting, sensorToVE, courier);
        courier = -1;
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
