#include <train_manager.h>


void TrainManager(){
	tid_t TTasks[TRAIN_SIZE];
	int i;
	for(i = 0; i < TRAIN_SIZE; i++){
		TTasks[i] = -1;
	}

  tid_t mytid = MyTid();

	int reply = 0;
	int r = RegisterAs(TRAIN_MANAGER_ID);
  	assert(r == 0);

  	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);

	while(true){
		tid_t tid_req;
		TMProtocol tmp;
    char buf[2];

		Receive(&tid_req, &tmp, sizeof(tmp));
		//Reply to Sender
		Reply(tid_req, &reply, sizeof(reply));

		switch(tmp.tmc){
			case TM_MOVE:
        buf[0] = tmp.arg2;
        buf[1] = tmp.arg1;
        PutStr(tx_tid, buf, 2);
				break;
			case TM_REVERSE:
        buf[0] = 0;
        buf[1] = tmp.arg1;
        PutStr(tx_tid, buf, 2);
        DelayCS(mytid, 100);
        buf[0] = 15;
        PutStr(tx_tid, buf, 2);
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}

	Exit();
}
