#include <switch_manager.h>


void PushSwitchToPrediction(tid_t pred, Switch *sw){
  int reply;
  PMProtocol pmp;
  pmp.pmc = PM_SWITCH;
  pmp.args = (void *)sw;
  pmp.size = 1;

  Send(pred, &pmp, sizeof(pmp), &reply, sizeof(reply));
}

void init_switch(tid_t pred_tid, tid_t sw_handler, Switch *slist, track_node *track){
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

  PushSwitchToPrediction(pred_tid, slist);
}

void UpdateSwitchTable(Switch *table, int sw, SwitchState dir){
  table[sw].state = dir;
}

void SwitchHandler(void *args){
	int reply = 0;
	tid_t tx1_writer = (tid_t)args;
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


void SwitchManager(void * args){
  track_node *track = (track_node *)args;
	Switch switchList[SWITCH_SIZE];

	int reply = 0;
	int r = RegisterAs(SWITCH_MANAGER_ID);
  assert(r == 0);
      tid_t pred_tid = WhoIs(PREDICTION_MANAGER_ID);
      assert(pred_tid >= 0);
  tid_t tx1_writer = WhoIs(IOSERVER_UART1_TX_ID);

  assert(tx1_writer >= 0);

  	tid_t sw_handler = CreateArgs(19, &SwitchHandler, (void *)tx1_writer);

  	init_switch(pred_tid, sw_handler, switchList, track);

  	while(true){
  		tid_t req_tid;
  		SWProtocol sw;

  		//Receive new SW request
  		Receive(&req_tid, &sw, sizeof(sw));

  		switch(sw.swr){
  			case SW_FLIP:
	  			//Send the command switch handler
	  			Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));
	  			//Update the UI and table
	  			UpdateSwitchTable(switchList, sw.sw, sw.dir);
          //Update Prediction
          PushSwitchToPrediction(pred_tid, switchList);
	  			Reply(req_tid, &reply, sizeof(reply));
  				break;
  			default:
  				assert(0 && "Bad Switch Command");
  		}
  	}
  Exit();
}
