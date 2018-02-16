#include <switch_manager.h>


void init_switch(tid_t tx2_writer, tid_t sw_handler, SW_Switch *slist){
	int reply = 0;
	SWProtocol sw;
	Cursor c;
	c.row = SWITCH_TABLE_ROW;
	c.col = SWITCH_TABLE_COL;

	//draw the ui table
	char *table = SWITCH_TABLE_STRING;
	WriteStringUART2(tx2_writer, table, &c);
	c.row+=3;
	c.col+=9;

	//Send Commands to SwitchHandler
	sw.dir = SW_CURVE;
	int i;
	for(i = NORMAL_SWITCH_SIZE_LOW; i <= NORMAL_SWITCH_SIZE_HIGH; ++i){
		slist[i] = SW_CURVE;
		sw.sw = i;
		Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));
		WriteStringUART2(tx2_writer, "C", &c);
		c.row++;
	}

	c.row++;
	for(i = SPECIAL_SWITCH_SIZE_LOW; i <= SPECIAL_SWITCH_SIZE_HIGH; ++i){
		slist[i] = SW_STRAIGHT+(i%2);
		sw.dir = SW_STRAIGHT+(i%2);
		sw.sw = i;
		Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));
		if(i%2){
			WriteStringUART2(tx2_writer, "C", &c);	
		}else{
			WriteStringUART2(tx2_writer, "S", &c);
		}
		c.row++;
	}
}

void UpdateSwitchTable(tid_t tx2_writer, SW_Switch *table, int sw, SW_Switch dir){
	Cursor c;
	c.row = SWITCH_TABLE_ROW + sw;
	c.col = SWITCH_TABLE_COL + 9;

	if(sw >= SPECIAL_SWITCH_SIZE_LOW){
		c.row++;
	}

	table[sw] = dir;
	if(dir == SW_CURVE){
		WriteStringUART2(tx2_writer, "C", &c);	
	}
	else{
		WriteStringUART2(tx2_writer, "S", &c);
	}
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

void SwitchManager(){
	SW_Switch SwitchList[SWITCH_SIZE];

	int reply = 0;
	int r = RegisterAs(SWITCH_MANAGER_ID);
  assert(r == 0);
  tid_t tx1_writer = WhoIs(WRITERSERVICE_UART1_ID);
  tid_t tx2_writer = WhoIs(WRITERSERVICE_UART2_ID);

  assert(tx1_writer >= 0);
  assert(tx2_writer >= 0);

  	tid_t sw_handler = CreateArgs(19, &SwitchHandler, (void *)tx1_writer);

  	init_switch(tx2_writer, sw_handler, SwitchList);

  	while(true){
  		tid_t req_tid;
  		SWProtocol sw;

  		//Receive new SW request
  		Receive(&req_tid, &sw, sizeof(sw));
  		Reply(req_tid, &reply, sizeof(reply));

  		//Send the command switch handler
  		Send(sw_handler, &sw, sizeof(sw), &reply, sizeof(reply));

  		//Update the UI and table
  		UpdateSwitchTable(tx2_writer, SwitchList, sw.sw, sw.dir);
  	}
  Exit();
}
