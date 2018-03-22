#include <user/train/waiting_room.h>
#include <terminal_manager.h>
#include <track_data.h> //TODO: REMOVE THIS

CIRCULAR_BUFFER_DEF(eg_cb, EventGroup, EVENT_GROUP_BUFFER_SIZE);
CIRCULAR_BUFFER_DEF(ve_key_cb, int, VE_KEY_BUFFER_SIZE);

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

  //Initial Subscribe
  Send(sn_pub, &sn_sub, sizeof(sn_sub), &r, sizeof(r));

  while (true) {
    //Send(sn_pub, &sn_sub, sizeof(sn_sub), &event, sizeof(event));
    Receive(&sn_pub, &event, sizeof(event));
    Reply(sn_pub, &r, sizeof(r));
    SendSensorDelta(wr_tid, cs_tid, my_tid, event.sensors, prev.sensors);
    memcpy(&prev, &event, sizeof(event));
  }

  Exit();
}


static void SwitchSubscriber() {
  int r;
  SWSubscribe sw_sub;
  WRRequest event;
  tid_t sw_pub, wr_tid;

  sw_pub = WhoIs(SWITCH_PUBLISHER_ID);
  assert(sw_pub > 0);

  wr_tid = MyParentTid();
  assert(wr_tid > 0);

  sw_sub.swr = SW_SUBSCRIBE;
  event.type = WR_RE;

  //Initial Subscribe
  Send(sw_pub, &sw_sub, sizeof(sw_sub), &r, sizeof(r));

  while (true) {
    //Send(sw_pub, &sw_sub, sizeof(sw_sub), &event.data.re, sizeof(RawEvent));
    Receive(&sw_pub, &event.data.re, sizeof(RawEvent));
    Reply(sw_pub, &r, sizeof(r));
    Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void TrainSubscriber() {
  int r;
  TSubscribe tsub;
  WRRequest event;
  tid_t tr_pub, wr_tid;

  tr_pub = WhoIs(TRAIN_PUBLISHER_ID);
  assert(tr_pub > 0);

  wr_tid = MyParentTid();
  assert(wr_tid > 0);

  tsub.tc = T_SUBSCRIBE;
  event.type = WR_RE;

  //Intial Subscribe
  Send(tr_pub, &tsub, sizeof(tsub), &r, sizeof(r));

  while (true) {
    //Send(tr_pub, &tsub, sizeof(tsub), &event.data.re, sizeof(RawEvent));
    Receive(&tr_pub, &event.data.re, sizeof(RawEvent));
    Reply(tr_pub, &r, sizeof(r));
    Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

static void VirtualEventSubscriber(){
  int r;
  VESubscribe vsub;
  WRRequest event;
  tid_t ve_pub, wr_tid;

  ve_pub = WhoIs(VIRTUAL_PUBLISHER_ID);
  assert(ve_pub > 0);

  wr_tid = MyParentTid();
  assert(wr_tid > 0);

  vsub.type = VER_SUBSCRIBE;
  event.type = WR_VE;
  
  //Initial Subscribe
  Send(ve_pub, &vsub, sizeof(vsub), &r, sizeof(r));

  while(true){
    //Send(ve_pub, &vsub, sizeof(vsub), &event.data.ve, sizeof(VirtualEvent));
    Receive(&ve_pub, &event.data.ve, sizeof(event.data.ve));
    Reply(ve_pub, &r, sizeof(r));
    Send(wr_tid, &event, sizeof(event), &r, sizeof(r));
  }

  Exit();
}

// ------------- WAITING ROOM SERVER ---------------- //

static void reset_waiting_room(VirtualEvent *ve){
  ve->type = VE_NONE;
}

void TimeoutWR_VE(void *args){
  WRRequest wrr;
  int r;

  tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
  tid_t my_tid = MyTid();
  tid_t wr_tid = MyParentTid();
  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(cs_tid > 0 && my_tid > 0 && wr_tid > 0 && tm_tid > 0);

  wrr.type = WR_TO;
  wrr.data.ve = *(VirtualEvent *) args;

  // TMLogStrf(tm_tid, "Timestart for location %s\n", wrr.data.ve.event.train_at.node->name);
  Delay(cs_tid, my_tid, wrr.data.ve.timeout);
  // TMLogStrf(tm_tid, "Timeout for location %s\n", wrr.data.ve.event.train_at.node->name);

  Send(wr_tid, &wrr, sizeof(wrr), &r, sizeof(r));
  Exit();
}

static void handle_ve_tr_at(VirtualEvent ve, VirtualEvent *waiting, ve_key_cb *sensorToVE){
  int key;

  key = ve.key;
  assert(key >= 0);

  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  if(waiting[key].type == VE_REG){
    //Spawn a timeout handler  
    CreateArgs(27, &TimeoutWR_VE, (void *)&ve, sizeof(VirtualEvent)); //TODO: UPDATE PRIORITY    
  }else{
    // TMLogStrf(tm_tid, "Already handled location %s\n", ve.event.train_at.node->name);    
  }
}

static void handle_ve_reg(VirtualEvent ve, VirtualEvent *waiting, ve_key_cb *sensorToVE){
  int sensor = ve.event.train_at.node->num;
  int key = ve.key;

  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  assert(sensor >= 0);
  assert(key >= 0);

  ve_key_cb_push(&sensorToVE[sensor], key);
  waiting[key] = ve;
  // TMLogStrf(tm_tid, "VRE on %s\n", ve.event.train_at.node->name);
}

void HandleWR_VE(WRRequest *event, VirtualEvent *waiting, ve_key_cb *sensorToVE){
  switch(event->data.ve.type){
    case VE_TR_AT:
      handle_ve_tr_at(event->data.ve, waiting, sensorToVE);
      break;
    case VE_REG:
      handle_ve_reg(event->data.ve, waiting, sensorToVE);
      break;
    default:
      assert(0 && "Bad Virtual Event");
  }
}

static void handle_re_se(RawEvent re, VirtualEvent *waiting, ve_key_cb *sensorToVE, eg_cb *dataBuf){
  EventGroup eg;
  int sensor, key;

  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  sensor = re.event.se_event.id;
  assert(sensor >= 0);

  if(sensorToVE[sensor].size > 0){
    //Possible conflict, but we'll assume they all ran over it
    while(ve_key_cb_pop(&sensorToVE[sensor], &key) != CB_E_EMPTY){
      eg.type = waiting[key].type == VE_REG ? VRE_RE : VRE_VE_RE; 
      eg.re = re;
      eg.ve = waiting[key];
      eg_cb_push(dataBuf, eg);
      reset_waiting_room(&waiting[key]);
      // TMLogStrf(tm_tid, "VRE RE or VRE VE RE on %d\n", re.event.se_event.id);
    }
  }else{
    //Just an RE
    eg.type = RE;
    eg.re = re;
    eg_cb_push(dataBuf, eg);
    // TMLogStrf(tm_tid, "RE on %d\n", re.event.se_event.id);
  }
}

void HandleWR_RE(WRRequest *event, VirtualEvent *waiting, ve_key_cb *sensorToVE, eg_cb *dataBuf){
  EventGroup eg;

  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  switch(event->data.re.type){
    case RE_SE:
      handle_re_se(event->data.re, waiting, sensorToVE, dataBuf);
      break;
    case RE_SW:
    case RE_TR_CMD:
      eg.type = RE;
      eg.re = event->data.re;
      eg_cb_push(dataBuf, eg);
      // TMLogStrf(tm_tid, "RE NOT SENSOR\n");
      break;
    default:
      assert(0 && "Bad Raw Event");
  }
}

static void handle_to_tr_at(VirtualEvent ve, VirtualEvent *waiting, ve_key_cb *sensorToVE, eg_cb *dataBuf){
  EventGroup eg;
  int sensor, key, size, i;

  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  sensor = ve.event.train_at.node->num;
  assert(sensor >= 0);

  if(sensorToVE[sensor].size > 0){
    size = sensorToVE[sensor].size;
    //Remove Virtual Event Due to Timeout
    for(i = 0; i < size; i++){
      ve_key_cb_pop(&sensorToVE[sensor], &key);
      if(key == ve.key){
        eg.type = VRE_VE;
        eg.ve = ve;
        eg_cb_push(dataBuf, eg);
        reset_waiting_room(&waiting[key]);
        TMLogStrf(tm_tid, "VRE VE on %s\n", ve.event.train_at.node->name);
        break;
      }
      else{
        ve_key_cb_push(&sensorToVE[sensor], key);
      }
    }
  }else{
    //Vitual Event Already Handled
    // TMLogStrf(tm_tid, "Timeout Ignored\n");
  } 
}

void HandleWR_TO(WRRequest *event, VirtualEvent *waiting, ve_key_cb *sensorToVE, eg_cb *dataBuf){

  switch(event->data.ve.type){
    case VE_TR_AT:
      handle_to_tr_at(event->data.ve, waiting, sensorToVE, dataBuf);
      break;
    default:
      assert(0 && "Timeout from bad data");
  }
}

void init_waiting_room(ve_key_cb *map){
  int i;
  for(i = 0; i < SENSOR_SIZE; i++){
    ve_key_cb_init(&map[i]);
  }
}

void test_waiting_room(){
  int r;
  tid_t cs_tid, my_tid, wr_tid, tm_tid, vp_tid;
  VERequest ver;
  VirtualEvent ve;

  my_tid = MyTid();
  wr_tid = MyParentTid();
  cs_tid = WhoIs(CLOCKSERVER_ID);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  vp_tid = WhoIs(VIRTUAL_PROVIDER_ID);

  assert(my_tid > 0 && wr_tid > 0 && cs_tid > 0 && tm_tid > 0 && vp_tid > 0);

  ve.type = VE_TR_AT;
  ve.timestamp = Time(cs_tid, my_tid) + 500;
  ve.timeout = 500;
  ve.depend = 38;
  ve.event.train_at.train_num = 24;
  ve.event.train_at.node = &TRACK[4];

  ver.type = VER_REGISTER;
  ver.ve = ve;

  while(true){
    ver.ve.timestamp = Time(cs_tid, my_tid) + 500;
    TMLogStrf(tm_tid, "Sending train %d at %s\n", ver.ve.event.train_at.train_num, ver.ve.event.train_at.node->name);
    Send(vp_tid, &ver, sizeof(ver), &r, sizeof(r));
    Delay(cs_tid, my_tid, 1200);  //Every 120 seconds
  }

  Exit();
}

void WaitingRoom(){
  int r;
  WRRequest event;
  EventGroup data;
  tid_t req_tid;
  tid_t courier = -1;

  VirtualEvent waiting[KEY_SIZE];
  ve_key_cb sensorToVE[SENSOR_SIZE];
  eg_cb dataBuf;
  eg_cb_init(&dataBuf);
  init_waiting_room(sensorToVE);

  r = RegisterAs(WAITING_ROOM_ID);
  assert(r == 0);
  tid_t my_tid = MyTid();

  Create(27, &TrainProvider);
  Create(27, &SwitchProvider);
  Create(27, &SensorProvider);
  Create(27, &VirtualProvider);

  Create(29, &TrainSubscriber);
  Create(29, &SwitchSubscriber);
  Create(29, &SensorSubscriber);
  Create(29, &VirtualEventSubscriber);

  while(true){

    if(dataBuf.size > 0 && courier > 0){
      eg_cb_pop(&dataBuf, &data);
      Reply(courier, &data, sizeof(data));
      courier = -1;
    }

    Receive(&req_tid, &event, sizeof(event));    

    switch(event.type){
      case WR_VE:
        Reply(req_tid, &r, sizeof(r));
        HandleWR_VE(&event, waiting, sensorToVE);
        break;
      case WR_RE:
        Reply(req_tid, &r, sizeof(r));
        HandleWR_RE(&event, waiting, sensorToVE, &dataBuf);
        break;
      case WR_TO:
        Reply(req_tid, &r, sizeof(r)); 
        HandleWR_TO(&event, waiting, sensorToVE, &dataBuf);
	break;
      case WR_CE:
        courier = req_tid;
        break;
      default:
        assert(0 && "bad request");
    }
  }

  Exit();
}
