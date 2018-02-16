#include <train_manager.h>

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

void parse_args(char *args, int size, int *parsed){
	int nargs = 0;
	int argh = 0;	//Head and Tail
	int argt = 0;

	while(argh < size){
		if(args[argt] != ' ' && argt < size){
			++argt;
		}else{
			if(argh != argt){	//non-empty arg
				//Push arg to tmp
				*parsed = str2int(args, argh, argt, 10);
				parsed++;
				nargs++;
			}
			++argt;
			argh = argt;
		}
	}
}

//<speed> <train>
void parse_tr(TMProtocol *tmp, char *command, int *csize){
	int args[2];
	parse_args(tmp->command, tmp->size, args);

	command[0] = (char)args[1];	//args[1] is train speed
	command[1] = (char)args[0];
	*csize = 2;		
}

//<train>
void parse_rv(TMProtocol *tmp, char *command, int *csize){
	int args[2];
	parse_args(tmp->command, tmp->size, args);

	command[0] = (char)args[0];	//Train
	*csize = 1;
}

//<dir> <switch>
void parse_sw(tid_t sw_tid, TMProtocol *tmp){
	int args[2];
	int reply;
	parse_args(tmp->command, tmp->size, args);

	SWProtocol sw;
	sw.sw = args[0];
	sw.dir = args[1] == 'C' ? TR_CURVE : TR_STRAIGHT; 
	Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
}

void TrainManager(){
	int reply = 0;
	int r = RegisterAs(TRAIN_MANAGER_ID);
  	assert(r == 0);

  	tid_t sw_tid = WhoIs(SWITCH_MANAGER_ID);
  	assert(sw_tid >= 0);

  	// tid_t rx_tid = WhoIs(IOSERVER_UART1_RX_ID);
  	// assert(rx_tid >= 0);
  	// tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	// assert(tx_tid >= 0);

  	char command[4];
  	int csize;

	while(true){
		tid_t tid_req;
		TMProtocol tmp;

		Receive(&tid_req, &tmp, sizeof(tmp));

		switch(tmp.trc){
			case TR_TRAIN:
				parse_tr(&tmp, command, &csize);
				assert(0 && "TR_TRAIN");
				break;
			case TR_REVERSE:
				parse_rv(&tmp, command, &csize);
				assert(0 && "TR_REVERSE");
				break;
			case TR_SWITCH:
				parse_sw(sw_tid, &tmp);
				break;
			case TR_SENSOR:
				assert(0 && "TR_SENSOR");
			default:
				assert(0 && "Bad Train Command");
				break;
		}

		//Reply to Sender
		Reply(tid_req, &reply, sizeof(reply));
	}
}
