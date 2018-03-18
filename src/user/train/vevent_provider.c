#include <user/train/vevent_provider.h>

/*
void SwitchHandler(void *args){
  int reply = 0;
  tid_t tx1_writer = *(tid_t *)args;
  tid_t my_tid = MyTid();

  assert(cs_tid >= 0);

  char sol_command[1];
  char sw_command[2];

  sol_command[0] = TURN_SOLENOID_OFF;

  while(true){
    tid_t req_tid;
    SWProtocol sw;

    Receive(&req_tid, &sw, sizeof(sw));
    Reply(req_tid, &reply, sizeof(reply));

  }

  Exit();
}

void SwitchPublisher(){
  int reply;
  tid_t req_tid, sub;
  SWSubscribe sws;

  tid_cb subscribers;
  tid_cb_init(&subscribers);

  int r = RegisterAs(SWITCH_PUBLISHER_ID);
  assert(r == 0);

  while(true){
    Receive(&req_tid, &sws, sizeof(sws));

    switch(sws.swr){
      case SW_NOTIFY:
        Reply(req_tid, &reply, sizeof(reply));
        NOTIFY(&subscribers, &sub, sws.swp, sizeof(sws.swp));
        break;
      case SW_SUBSCRIBE:
        tid_cb_push(&subscribers, req_tid);
        break;
      default:
        assert(0 && "Bad Command");
    }
  }

  Exit();
}

void SwitchUpdateCourier(){
  SWProtocol swp, data;
  SWSubscribe sws;
  int reply;

  tid_t pub_tid = WhoIs(SWITCH_PUBLISHER_ID);
  tid_t sw_tid = MyParentTid();

  while(true){
    swp.swr = SW_NOTIFY_READY;
    Send(sw_tid, &swp, sizeof(swp), &data, sizeof(data));

    sws.swr = SW_NOTIFY;
    sws.swp = data;
    Send(pub_tid, &sws, sizeof(sws), &reply, sizeof(reply));
  }

  Exit();
}

void SwitchProvider(){
  bool courierFlag = false;
  bool switchFlag = false;

  int reply = 0;
  int r = RegisterAs(SWITCH_PROVIDER_ID);
  assert(r == 0);

  tid_t sw_handler = CreateArgs(29, &SwitchHandler, &tx1_writer, sizeof(tx1_writer));
  Create(29, &SwitchPublisher);
  tid_t suc_tid = Create(29, &SwitchUpdateCourier);
  Create(19, &TestSWPublisher); //TODO: Remove this

  while(true){
    tid_t req_tid;
    SWProtocol sw, data;

    if(courierFlag && switchFlag){
      courierFlag = false;
      switchFlag = false;
      Reply(suc_tid, &data, sizeof(data));
    }

    //Receive new SW request
    Receive(&req_tid, &sw, sizeof(sw));

    switch(sw.swr){
      case SW_FLIP:
        //Send the command switch handler
        Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));
        Reply(req_tid, &reply, sizeof(reply));
        switchFlag = true;
        data = sw;  //persist data until read
        break;
      case SW_NOTIFY_READY:
        courierFlag = true;
        break;
      default:
        assert(0 && "Bad Switch Command");
    }
  }
  Exit();
}
*/
