#include <train_provider.h>

void TrainPublisher(){
	int reply;
	tid_t req_tid, sub;
	TSubscribe ts;

	tid_cb subscribers;
	tid_cb_init(&subscribers);

	int r = RegisterAs(TRAIN_PUBLISHER_ID);
	assert(r == 0);

	while(true){
		Receive(&req_tid, &ts, sizeof(ts));

		switch(ts.tc){
			case T_NOTIFY:				
				Reply(req_tid, &reply, sizeof(reply));
				NOTIFY(&subscribers, &sub, ts.re, sizeof(ts.re))
				break;
			case T_SUBSCRIBE:
				tid_cb_push(&subscribers, req_tid);
				break;
			default:
				assert(0 && "Bad train command");
		}
	}

	Exit();	
}

void TWriteTask(void *args){
	TrainProtocol cmd = *(TrainProtocol *)args;
	char buf[2];
	int reply;
	TSubscribe ts;

	tid_t my_tid = MyTid();
	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);
  	tid_t pub_tid = WhoIs(TRAIN_PUBLISHER_ID);
  	assert(pub_tid >= 0);
  	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	assert(cs_tid >= 0);

	//Handle Command
	switch(cmd.tc){
		case T_MOVE:
			buf[0] = cmd.arg2;
			buf[1] = cmd.arg1;
			PutStr(tx_tid, buf, 2);
			break;
    default:
      assert(0);
      break;
	}

	//Send the Updated Command to the publisher
	ts.tc = T_NOTIFY;
	ts.re.type = RE_TR_CMD;
	ts.re.timestamp = Time(cs_tid, my_tid);
	ts.tp.event.tr_cmd_event = cmd;
	Send(pub_tid, &ts, sizeof(ts), &reply, sizeof(reply));

	Exit();
}

void TestTMPublisher(){
	TSubscribe ts;
	TrainProtocol res;
	tid_t pub_tid = WhoIs(TRAIN_PUBLISHER_ID);
	assert(pub_tid >= 0);
	tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	assert(tm_tid >= 0);


	ts.tc = T_SUBSCRIBE;

	while(true){
		Send(pub_tid, &ts, sizeof(ts), &res, sizeof(res));
		TMLogStrf(tm_tid, "Command: %d, Arg: %d\n", res.tc, res.arg1);
	}

	Exit();
}

void TrainProvider(){
	int reply = 0;
	TrainProtocol tp;
	tid_t tid_req;

	int r = RegisterAs(TRAIN_PROVIDER_ID);
	assert(r == 0);

	tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	assert(tm_tid >= 0);
	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
	assert(tx_tid >= 0);

  	//Construct the Train Publisher
  	Create(29, &TrainPublisher);

	while(true){
		Receive(&tid_req, &tp, sizeof(tp));

		switch(tp.tc){
			case T_MOVE:
				Reply(tid_req, &reply, sizeof(reply));
				CreateArgs(29, &TWriteTask, &tp, sizeof(tp));
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}

	Exit();
}
