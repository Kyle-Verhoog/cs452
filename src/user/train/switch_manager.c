#include <switch_manager.h>


void init_switch(tid_t tx1_tid, tid_t tx2_tid, tid_t sw_handler, SW_Switch *slist){
	int reply = 0;
	SWProtocol sw;
	Cursor c;
	c.row = SWITCH_TABLE_ROW;
	c.col = SWITCH_TABLE_COL;

	//draw the ui table
	char *table = SWITCH_TABLE_STRING;
	//WriteStringUART2(tx2_tid, table, &c); //TODO: IMPL THIS
	c.row+=3;
	c.col+=9;

	//Send Commands to SwitchHandler
	sw.dir = SW_CURVE;
	int i;
	for(i = NORMAL_SWITCH_SIZE_LOW; i <= NORMAL_SWITCH_SIZE_HIGH; ++i){
		slist[i] = SW_CURVE;
		sw.sw = i;
		Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));
		//WriteStringUART2(tx2_tid, "C", &c); //TODO: IMPL THIS
		c.row++;
	}

	c.row++;
	for(i = SPECIAL_SWITCH_SIZE_LOW; i <= SPECIAL_SWITCH_SIZE_HIGH; ++i){
		slist[i] = SW_STRAIGHT+(i%2);
		sw.dir = SW_STRAIGHT+(i%2);
		sw.sw = i;
		Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));
		if(i%2){
			//WriteStringUART2(tx2_tid, "C", &c); //TODO: IMPL THIS	
		}else{
			//WriteStringUART2(tx2_tid, "S", &c); //TODO: IMPL THIS	
		}
		c.row++;
	}
}

void UpdateSwitchTable(SW_Switch *table, int sw, SW_Switch dir){
	Cursor c;
	c.row = SWITCH_TABLE_ROW + sw;
	c.col = SWITCH_TABLE_COL + 9;

	if(sw >= SPECIAL_SWITCH_SIZE_LOW){
		c.row++;
	}

	table[sw] = dir;
	if(dir == SW_CURVE){
		//WriteStringUART2(tx2_tid, "C", &c); //TODO: IMPL THIS	
	}
	else{
		//WriteStringUART2(tx2_tid, "S", &c); //TODO: IMPL THIS	
	}
}

void SwitchHandler(void *args){
	int reply = 0;
	tid_t tx1_tid = (tid_t)args;
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

		//WriteCommandUART1(tx1_tid, sw_command, 2);	//TODO: IMPL THIS
		//WriteCommandUART1(tx1_tid, sol_command, 1);	//TODO: IMPL THIS
		PRINTF("SWITCHING SWITCHES: %d", sw.sw);
		Delay(cs_tid, my_tid, 15);	//delay 150 ms
	}

  Exit();
}

void SwitchManager(){
	SW_Switch SwitchList[SWITCH_SIZE];

	int reply = 0;
	int r = RegisterAs(SWITCH_MANAGER_ID);
  	assert(r == 0);
  	tid_t rx1_tid = WhoIs(IOSERVER_UART1_RX_ID);
  	//assert(rx1_tid >= 0);
  	tid_t tx1_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	//assert(tx1_tid >= 0);
  	tid_t rx2_tid = WhoIs(IOSERVER_UART2_RX_ID);
  	//assert(rx2_tid >= 0);
  	tid_t tx2_tid = WhoIs(IOSERVER_UART2_TX_ID);
  	//assert(tx2_tid >= 0);

  	tid_t sw_handler = CreateArgs(19, &SwitchHandler, (void *)&tx1_tid);

  	init_switch(tx1_tid, tx2_tid, sw_handler, SwitchList);

  	while(true){
  		tid_t req_tid;
  		SWProtocol sw;

  		//Receive new SW request
  		Receive(&req_tid, &sw, sizeof(sw));
  		Reply(req_tid, &reply, sizeof(reply));

  		//Send the command switch handler
  		Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));

  		//Update the UI and table
  		UpdateSwitchTable(SwitchList, sw.sw, sw.dir);
  	}
  Exit();
}
