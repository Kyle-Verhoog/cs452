#include <railway_manager.h>


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

  	CreateArgs(29, &PredictionManager, (void *)track, sizeof(track_node *));
  	tid_t tm_tid = CreateArgs(29, &TrainManager, (void *)track, sizeof(track_node *));
  	tid_t sw_tid = CreateArgs(29, &SwitchManager, (void *)track, sizeof(track_node *));

  	//  CreateArgs(29, &SensorManager, (void *)track, sizeof(track_node *));
  	// assert(tm_tid >= 0);
  	// assert(sw_tid >= 0);

	while(true){
		tid_t tid_req;
		RWProtocol rwp;

		Receive(&tid_req, &rwp, sizeof(rwp));

		switch(rwp.rwc){
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
