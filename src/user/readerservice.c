#include<readerservice.h>

void parse_and_send(char *command, int size){
	if(command[0] == 'q'){
		//Quit
	}
	else if(command[0] == 'r' && command[1]=='v'){
		//Reverse
	}
	else if(command[0] == 't' && command[1]=='r'){
		//Move Train
	}
	else if(command[0] == 's' && command[1]=='w'){
		//Switch
	}
	else{
		//Bad Command
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
					parse_and_send(command, csize);	
				}				
				csize = 0;
			default:
				csize++;
		}
	}
}
