#include <switch_manager.h>
#include <prediction_manager.h>

void init_switch(tid_t sw_handler, Switch *slist){
	int reply = 0;
	SWProtocol swp;

	//Send Commands to SwitchHandler
	swp.swr = SW_FLIP;
	swp.dir = SW_CURVE;
	int node = 0;
	int i;
	for(i = NORMAL_SWITCH_SIZE_LOW; i <= NORMAL_SWITCH_SIZE_HIGH; ++i){
		while(true){
			if(track[node].type == NODE_BRANCH){
				slist[i].branch = &track[node];
				slist[i].merge = &track[node+1];
				node+=2;
				break;
			}
			node++;
		}
		slist[i].state = SW_CURVE;
		swp.sw = i;
		Send(sw_handler, &swp, sizeof(swp), &reply, sizeof(reply));
	}

	// c.row++;
	for(i = SPECIAL_SWITCH_SIZE_LOW; i <= SPECIAL_SWITCH_SIZE_HIGH; ++i){
		while(true){
			if(track[node].type == NODE_BRANCH){
				slist[i].branch = &track[node];
				slist[i].merge = &track[node+1];
				node+=2;
				break;
			}
			node++;
		}
		slist[i].state = SW_STRAIGHT+(i%2);
		swp.dir = SW_STRAIGHT+(i%2);
		swp.sw = i;
		Send(sw_handler, &swp, sizeof(swp), &reply, sizeof(reply));
	}
}

void UpdateSwitchTable(Switch *table, int sw, SwitchState dir){
  table[sw].state = dir;
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
		Reply(req_tid, &reply, sizeof(reply));

		sw_command[0] = sw.dir;
		sw_command[1] = sw.sw;

		WriteCommandUART1(tx1_writer, sw_command, 2);
		Delay(cs_tid, my_tid, 15);	//delay 150 ms
		WriteCommandUART1(tx1_writer, sol_command, 1);
	}

  Exit();
}

void SwitchPublisher(void *args){
  void *data;
  int reply;
  tid_t req_tid;
  SWProtocol swp;
  Switch *switches = *(Switch**)args;

  tid_cb subscribers;
  tid_cb_init(&subscribers);

  int r = RegisterAs(SWITCH_PUBLISHER_ID);
  assert(r == 0);

  while(true){
    Receive(&req_tid, &swp, sizeof(swp));

    switch(swp.swr){
      case SW_NOTIFY:
        Reply(req_tid, &reply, sizeof(reply));
        Notify(&subscribers);
        break;
      case SW_SUBSCRIBE:
        tid_cb_push(&subscribers, req_tid);
        break;
      case SW_GET_ALL:
        data = (void *)switches;
        Reply(req_tid, &data, sizeof(data));
        break;
      default:
        assert(0 && "Bad Command");
    }
  }

  Exit();
}

// void SwitchUpdateCourier(){
//   SMProtocol swp;
//   int reply;

//   tid_t pub_tid = WhoIs(SWITCH_PUBLISHER_ID);

//   swp.smr = SW_NOTIFY;

//   Send(pub_tid, &swp, sizeof(swp), &reply, sizeof(reply));
//   Exit();
// }

void SwitchUpdateCourier(){
  SMProtocol swp;
  int reply;

  tid_t pub_tid = WhoIs(SWITCH_PUBLISHER_ID);
  tid_t sw_tid = MyParentTid();

  while(true){
    swp.smr = SW_NOTIFY_READY;
    Send(sw_tid, &swp, sizeof(swp), &reply, sizeof(reply));  

    swp.smr = SW_NOTIFY;
    Send(pub_tid, &swp, sizeof(swp), &reply, sizeof(reply));  
  }
  
  Exit();
}

void SwitchManager(){
	Switch switchList[SWITCH_SIZE];
  Switch *swl = switchList;
  bool courierFlag = false;
  bool switchFlag = false;

	int reply = 0;
	int r = RegisterAs(SWITCH_MANAGER_ID);
  assert(r == 0);
  tid_t tx1_writer = WhoIs(IOSERVER_UART1_TX_ID);
  assert(tx1_writer >= 0);

  // TODO: make switchList a global?
  	tid_t sw_handler = CreateArgs(29, &SwitchHandler, &tx1_writer, sizeof(tx1_writer));
    CreateArgs(29, &SwitchPublisher, &swl, sizeof(Switch *));
    tid_t suc_tid = Create(29, &SwitchUpdateCourier);
    init_switch(sw_handler, switchList);


  	while(true){
  		tid_t req_tid;
  		SWProtocol sw;

      if(courierFlag && switchFlag){
        courierFlag = false;
        switchFlag = false;
        Reply(suc_tid, &reply, sizeof(reply));
      }

  		//Receive new SW request
  		Receive(&req_tid, &sw, sizeof(sw));

  		switch(sw.swr){
  			case SW_FLIP:
	  			//Send the command switch handler
	  			Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));
	  			//Update the UI and table
	  			UpdateSwitchTable(switchList, sw.sw, sw.dir);
          //TODO: CHANGE THIS
          //Create(29, &SwitchUpdateCourier);
	  			Reply(req_tid, &reply, sizeof(reply));
          switchFlag = true;
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
