#include<readerservice.h>

void parse_and_send(tid_t tm, char *command, int size){
	RWProtocol rwp;
	int reply;

	if(command[0] == 'q'){
		//Quit
	}
	else if(command[0] == 'r' && command[1]=='v'){
		//Reverse
		rwp.rwc = RW_REVERSE;
		rwp.command = command+2;
		rwp.size = size-2;
		Send(tm, &rwp, sizeof(rwp), &reply, sizeof(reply));
	}
	else if(command[0] == 't' && command[1]=='r'){
		//Move Train
		rwp.rwc = RW_TRAIN;
		rwp.command = command+2;
		rwp.size = size-2;
		Send(tm, &rwp, sizeof(rwp), &reply, sizeof(reply));
	}
	else if(command[0] == 's' && command[1]=='w'){
		//Switch
		rwp.rwc = RW_SWITCH;
		rwp.command = command+2;
		rwp.size = size-2;
		Send(tm, &rwp, sizeof(rwp), &reply, sizeof(reply));
	}
	else{
		//Bad Command
		PRINTF("%c%c", command[0], command[1]);
		assert(0 && "bad command");
	}
}

void ReaderServiceUART2(){
	int r = RegisterAs(READERSERVICE_UART2_ID);
  	assert(r == 0);
	tid_t rx_tid = WhoIs(IOSERVER_UART2_RX_ID);
	assert(rx_tid >= 0);
	tid_t tx_tid = WhoIs(IOSERVER_UART2_TX_ID);
	assert(tx_tid >= 0);
	tid_t writer = WhoIs(WRITERSERVICE_UART2_ID);
	assert(writer >= 0);
	tid_t rm = WhoIs(RAILWAY_MANAGER_ID);
	assert(rm >= 0);

	char command[COMMAND_SIZE];
	int csize = 0;

	while(true){
		//Get and Writeback
		command[csize] = GetC(rx_tid);
		WriteCharUART2(writer, command[csize]);

		switch(command[csize]){
			case BACKSPACE:
				if(csize >= 2){
					csize -= 2;
				}else{
					csize = 0;
				}
				break;
			case CARRIAGE_RETURN:
				if(csize > 0){
					parse_and_send(rm, command, csize);	
				}				
				csize = 0;
				break;
			default:
				csize++;
		}
	}
}
