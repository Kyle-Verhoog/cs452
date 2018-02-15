#include<readerservice.h>

//Non inclusive tail
int str2int(char *args, int head, int tail, int base){
	int result = 0;
	int size = tail-head;
	int i;
	int dig = 1;
	for(i = 0; i < size; i++){
		result += (args[tail - i -1] - '0') * dig;
		dig *= base;
	}
	return result;
}

void parse_args(char *args, int size, TMProtocol *tmp){
	int nargs = 0;
	int args = 0;	//Start and End
	int arge = 0;

	while(args < size){
		if(args[arge] != ' ' && arge < size){
			++arge;
		}else{
			if(args != arge){	//non-empty arg
				//Push arg to tmp
				int val = str2int(args, args, arge, 10);
				if(nargs == 0){
					tmp->arg1 = val;
				}else if(nargs == 1){
					tmp->arg2 = val;
				}
				nargs++;
			}
			++arge;
			args = arge;
		}
	}
}

void parse_and_send(tid_t tm, char *command, int size){
	TMProtocol tmp;
	int reply;

	if(command[0] == 'q'){
		//Quit
	}
	else if(command[0] == 'r' && command[1]=='v'){
		//Reverse
		tmp.trc = TR_REVERSE;
		parse_args(command + 2, size - 2, &tmp);
	}
	else if(command[0] == 't' && command[1]=='r'){
		//Move Train
		tmp.trc = TR_TRAIN;
		parse_args(command + 2, size - 2, &tmp);
	}
	else if(command[0] == 's' && command[1]=='w'){
		//Switch
		tmp.trc = TR_SWITCH;
		parse_args(command + 2, size - 2, &tmp);
	}
	else{
		//Bad Command
		assert(0 && "bad command");
	}
	Send(tm, &tmp, sizeof(tmp), &reply, sizeof(reply));
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
	tid_t tm = WhoIs(TRAIN_MANAGER_ID);
	assert(tm >= 0);

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
					parse_and_send(tm, command, csize);	
				}				
				csize = 0;
			default:
				csize++;
		}
	}
}
