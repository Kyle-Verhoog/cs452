#include <train_manager.h>


void TrainManager(){
	tid_t TTasks[TRAIN_SIZE];
	int i;
	for(i = 0; i < TRAIN_SIZE; i++){
		TTasks[i] = -1;
	}

	int reply = 0;
	int r = RegisterAs(TRAIN_MANAGER_ID);
  	assert(r == 0);

  	// tid_t rx_tid = WhoIs(IOSERVER_UART1_RX_ID);
  	// assert(rx_tid >= 0);
  	// tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	// assert(tx_tid >= 0);

	while(true){
		tid_t tid_req;
		TMProtocol tmp;

		Receive(&tid_req, &tmp, sizeof(tmp));
		//Reply to Sender
		Reply(tid_req, &reply, sizeof(reply));

		switch(tmp.tmc){
			case TM_MOVE:
				assert(0 && "TR_TRAIN");
				break;
			case TM_REVERSE:
				assert(0 && "TR_REVERSE");
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}

	Exit();
}
