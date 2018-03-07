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

//<train>
void parse_tk(tid_t tm_tid, RWProtocol *rwp){
	int args[2];
	int reply;
	parse_args(rwp->command, rwp->size, args);

	TMProtocol tm;
	tm.tmc = TM_TRACK;
	tm.arg1 = args[0]; //train
	tm.arg2 = args[1]; //track
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
}

//<switch> <dir>
void parse_sw(tid_t sw_tid, RWProtocol *rwp){
	int reply;
  int arg1;
  char swd;
  char *r;

  if ((r = parse_i32(rwp->command, &arg1)) == 0 || arg1 < 0) {
    assert(0);
  }

  if ((r = parse_c(r, &swd)) == 0 || (swd != 'S' && swd != 'C')) {
    assert(0);
  }

	SWProtocol sw;
	sw.swr = SW_FLIP;
	sw.sw = arg1;
	sw.dir = swd == 'C' ? SW_CURVE : SW_STRAIGHT;
	Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
}

void RailwayInit(){
	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);
  	tid_t rx_tid = WhoIs(IOSERVER_UART1_RX_ID);
  	assert(rx_tid >= 0);

  	//Send Reset Commands to Terminal
  	//BLPutC(tx_tid, 'X');
  	BLPutC(tx_tid, 192);

  	//Flush the IO
  	FlushIO(tx_tid);
  	FlushIO(rx_tid);

  	//Send Go
  	PutC(tx_tid, 96);
}

void RailwayManager(){
	void *data;
	int reply = 0;
	int r = RegisterAs(RAILWAY_MANAGER_ID);
  	assert(r == 0);

  	//Reset The Train Controller
  	RailwayInit();

  	track_node track[TRACK_MAX];
  	init_tracka(track);

  	CreateArgs(29, &PredictionManager, (void *)track);
  	tid_t tm_tid = CreateArgs(29, &TrainManager, (void *)track);
  	tid_t sw_tid = CreateArgs(29, &SwitchManager, (void *)track);
  	CreateArgs(29, &SensorManager, (void *)track);
  	assert(tm_tid >= 0);
  	assert(sw_tid >= 0);

	while(true){
		tid_t tid_req;
		RWProtocol rwp;

		Receive(&tid_req, &rwp, sizeof(rwp));

		switch(rwp.rwc){
			case RW_TRAIN:
				parse_tr(tm_tid, &rwp);
				Reply(tid_req, &reply, sizeof(reply));
				break;
			case RW_REVERSE:
				parse_rv(tm_tid, &rwp);
				Reply(tid_req, &reply, sizeof(reply));
				break;
			case RW_SWITCH:
				parse_sw(sw_tid, &rwp);
				Reply(tid_req, &reply, sizeof(reply));
				break;
			case RW_TRACK:
				parse_tk(tm_tid, &rwp);
				Reply(tid_req, &reply, sizeof(reply));
				break;
			case RW_GET_TRACK:
				data = (void *)track;
				Reply(tid_req, &data, sizeof(data));
				break;
			case RW_SENSOR:
				assert(0 && "RW_SENSOR");
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}

	Exit();
}
