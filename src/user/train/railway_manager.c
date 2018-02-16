#include <railway_manager.h>

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
				//Push arg to rwp
				*parsed = str2int(args, argh, argt, 10);
				parsed++;
				nargs++;
			}
			++argt;
			argh = argt;
		}
	}
}

//<train> <speed>
void parse_tr(tid_t tm_tid, RWProtocol *rwp){
	int args[2];
	int reply;
	parse_args(rwp->command, rwp->size, args);

	TMProtocol tm;
	tm.tmc = TM_MOVE;
	tm.arg1 = args[0];	//train
	tm.arg2 = args[1];	//train speed
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
}

//<train>
void parse_rv(tid_t tm_tid, RWProtocol *rwp){
	int args[1];
	int reply;
	parse_args(rwp->command, rwp->size, args);

	TMProtocol tm;
	tm.tmc = TM_REVERSE;
	tm.arg1 = args[0];	//train
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
}

//<switch> <dir>
void parse_sw(tid_t sw_tid, RWProtocol *rwp){
	int args[2];
	int reply;
	parse_args(rwp->command, rwp->size, args);

	SWProtocol sw;
	sw.sw = args[0];
	sw.dir = args[1] == 'C' ? SW_CURVE : SW_STRAIGHT; 
	Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
}

void RailwayManager(){
	int reply = 0;
	int r = RegisterAs(RAILWAY_MANAGER_ID);
  	assert(r == 0);

  	tid_t tm_tid = WhoIs(TRAIN_MANAGER_ID);
  	assert(tm_tid >= 0);
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
		RWProtocol rwp;

		Receive(&tid_req, &rwp, sizeof(rwp));

		switch(rwp.rwc){
			case RW_TRAIN:
				parse_tr(tm_tid, &rwp);
				break;
			case RW_REVERSE:
				parse_rv(tm_tid, &rwp);
				break;
			case RW_SWITCH:
				parse_sw(sw_tid, &rwp);
				break;
			case RW_SENSOR:
				assert(0 && "RW_SENSOR");
			default:
				assert(0 && "Bad Train Command");
				break;
		}

		//Reply to Sender
		Reply(tid_req, &reply, sizeof(reply));
	}

	Exit();
}

