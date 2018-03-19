#include <switch_provider.h>
#include <prediction_manager.h>

void init_switch(){
  int reply = 0;
  SWProtocol swp;

  //Send Commands to the Switch Provider
  swp.swr = SW_FLIP;
  swp.dir = SW_CURVE;

  tid_t sw_tid = WhoIs(SWITCH_PROVIDER_ID); 
  int i;
  for(i = NORM_SW_LOW; i <= NORM_SW_HIGH; ++i){
    swp.sw = i;
    Send(sw_tid, &swp, sizeof(swp), &reply, sizeof(reply));
  }
  for(i = SPEC_SW_LOW; i <= SPEC_SW_HIGH; ++i){
    swp.sw = i;
    swp.dir = SW_STRAIGHT+(i%2);
    Send(sw_tid, &swp, sizeof(swp), &reply, sizeof(reply));
  }

  Exit();
}

void SwitchHandler(void *args){
	int reply = 0;
	tid_t tx1_writer = *(tid_t *)args;
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	tid_t my_tid = MyTid();

	assert(cs_tid >= 0);

  	char sol_command[1];
  	char sw_command[2];

  	sol_command[0] = TURN_SOLENOID_OFF;

	while(true){
		tid_t req_tid;
  	SWProtocol sw;

		Receive(&req_tid, &sw, sizeof(sw));
    assert(IS_VALID_ID(TID_ID(req_tid)));
		Reply(req_tid, &reply, sizeof(reply));

		sw_command[0] = sw.dir;
		sw_command[1] = sw.sw;

    PutStr(tx1_writer, sw_command, 2);
		Delay(cs_tid, my_tid, 15);	//delay 150 ms
    PutStr(tx1_writer, sol_command, 1);
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
        assert(IS_VALID_ID(TID_ID(req_tid)));
        Reply(req_tid, &reply, sizeof(reply));
        NOTIFY(&subscribers, &sub, sws.re, sizeof(sws.re));
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
  SWProtocol swp;
  SWSubscribe sws;
  RawEvent data;
  int reply;

  tid_t pub_tid = WhoIs(SWITCH_PUBLISHER_ID);
  assert(pub_tid > 0);
  tid_t sw_tid = MyParentTid();
  assert(sw_tid > 0);

  while(true){
    swp.swr = SW_NOTIFY_READY;
    Send(sw_tid, &swp, sizeof(swp), &data, sizeof(data));

    sws.swr = SW_NOTIFY;
    sws.re = data;
    Send(pub_tid, &sws, sizeof(sws), &reply, sizeof(reply));
  }
  
  Exit();
}

void TestSWPublisher(){
  SWSubscribe sws;
  SWProtocol res;

  tid_t pub_tid = WhoIs(SWITCH_PUBLISHER_ID);
  assert(pub_tid >= 0);
  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  sws.swr = SW_SUBSCRIBE;

  while(true){
    Send(pub_tid, &sws, sizeof(sws), &res, sizeof(res));
  }

  Exit();
}

void SwitchProvider(){
  tid_t req_tid;
  SWProtocol sw;
  RawEvent data;

  bool courierFlag = false;
  bool switchFlag = false;

	int reply = 0;
	int r = RegisterAs(SWITCH_PROVIDER_ID);
  assert(r == 0);
  tid_t my_tid = MyTid();
  tid_t tx1_writer = WhoIs(IOSERVER_UART1_TX_ID);
  tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(tx1_writer >= 0 && cs_tid > 0 && my_tid > 0);

	tid_t sw_handler = CreateArgs(29, &SwitchHandler, &tx1_writer, sizeof(tx1_writer));
  Create(29, &SwitchPublisher);
  tid_t suc_tid = Create(29, &SwitchUpdateCourier);
  // Create(19, &TestSWPublisher); //TODO: Remove this

  data.type = RE_SW;

	while(true){
    if(courierFlag && switchFlag){
      courierFlag = false;
      switchFlag = false;
      assert(IS_VALID_ID(TID_ID(suc_tid)));
      Reply(suc_tid, &data, sizeof(data));
    }

		//Receive new SW request
		Receive(&req_tid, &sw, sizeof(sw));

		switch(sw.swr){
			case SW_FLIP:
  			//Send the command switch handler
  			Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));
        assert(IS_VALID_ID(TID_ID(req_tid)));
  			Reply(req_tid, &reply, sizeof(reply));
        switchFlag = true;
        data.event.sw_event = sw;  //persist data until read
        data.timestamp = Time(cs_tid, my_tid);
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
