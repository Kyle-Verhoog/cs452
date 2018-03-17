#include <train_provider.h>
//#include <prediction_manager.h>
//#include <stoppingcalibration_test.h>


//tid_t tm_tid;

void NotifyTMSubscribers(tid_cb *subscribers, TrainProtocol cmd){
	tid_t sub;

	while(tid_cb_pop(subscribers, &sub) != CB_E_EMPTY){
		Reply(sub, &cmd, sizeof(cmd));
	}
}

void TrainPublisher(){
	int reply;
	tid_t req_tid, sub;
	TMSubscribe tms;

	tid_cb subscribers;
	tid_cb_init(&subscribers);

	int r = RegisterAs(TRAIN_PUBLISHER_ID);
	assert(r == 0);

	while(true){
		Receive(&req_tid, &tms, sizeof(tms));

		switch(tms.tmc){
			case TM_NOTIFY:				
				Reply(req_tid, &reply, sizeof(reply));
				NOTIFY(&subscribers, &sub, tms.tp, sizeof(tms.tp))
				break;
			case TM_SUBSCRIBE:
				tid_cb_push(&subscribers, req_tid);
				break;
			default:
				assert(0 && "Bad train command");
		}
	}

	Exit();	
}

// void TMWriteTask(void *args){
// 	char buf[2];
// 	int reply;
// 	TrainProtocol cmd;
// 	TMSubscribe tms;
// 	TrainDescriptor* td = *(TrainDescriptor **)args;

// 	tid_t mytid = MyTid();
// 	tid_t parentTid = MyParentTid();
// 	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
//   	assert(tx_tid >= 0);
//   	tid_t pub_tid = WhoIs(TRAIN_PUBLISHER_ID);
//   	assert(pub_tid >= 0);

//   	//Pop from instruction queue
// 	if(tc_cb_pop(&td->buf, &cmd) == CB_E_EMPTY){
// 		assert(0);
// 	}

// 	//Send the Updated Command to the publisher (TODO: Rare - Possible unwanted blocking)
// 	tms.tmc = TM_NOTIFY;
// 	tms.tp = cmd;
//   // TMLogStrf(tm_tid, "%x", MyTid());
// 	Send(pub_tid, &tms, sizeof(tms), &reply, sizeof(reply));

// 	//Handle Command
// 	switch(cmd.tc){
// 		case T_MOVE:
// 			buf[0] = cmd.arg2;
// 			buf[1] = cmd.arg1;
// 			PutStr(tx_tid, buf, 2);
// 			//Update the Descriptor
// 			td->gear = cmd.arg2;
// 			break;
// 		case T_REVERSE:
// 			buf[0] = 0;
// 	        buf[1] = cmd.arg1;
// 	        PutStr(tx_tid, buf, 2);
// 	        if(td->gear > 0){
// 	        	DelayCS(mytid, td->gear*10*2);	//Some arbitrary delay based on speed
// 	        }
// 	        buf[0] = 15;
// 	        PutStr(tx_tid, buf, 2);
// 	        buf[0] = td->gear;
// 	        PutStr(tx_tid, buf, 2);
// 	        td->dir = td->dir ? false:true;
// 	        if(td->node != NULL){
// 	        	td->node = td->node->reverse;	
// 	        }	        
// 			break;
// 		case T_DELAY:
// 			DelayCS(mytid, cmd.arg1);
// 			break;
// 		default:
// 			assert(0 && "Bad Train Command");
// 			break;
// 	}

// 	//Send to Train Manager to complete the task
// 	TMProtocol tmp;
// 	tmp.tmc = TM_TASK_COMPLETE;
// 	tmp.arg1 = td->id;
// 	Send(parentTid, &tmp, sizeof(TMProtocol), &reply, sizeof(reply));

// 	Exit();
// }

void TWriteTask(void *args){
	TrainProtocol cmd = *(TrainProtocol *)args;
	char buf[2];
	int reply;
	TMSubscribe tms;

	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);
  	tid_t pub_tid = WhoIs(TRAIN_PUBLISHER_ID);
  	assert(pub_tid >= 0);

	//Handle Command
	switch(cmd.tc){
		case T_MOVE:
			buf[0] = cmd.arg2;
			buf[1] = cmd.arg1;
			PutStr(tx_tid, buf, 2);
			break;		
		default:
			assert(0 && "Bad Train Command");
			break;
	}

	//Send the Updated Command to the publisher
	tms.tmc = TM_NOTIFY;
	tms.tp = cmd;
	Send(pub_tid, &tms, sizeof(tms), &reply, sizeof(reply));

	Exit();
}

void TestTMPublisher(){
	TMSubscribe tms;
	TrainProtocol res;
	tid_t pub_tid = WhoIs(TRAIN_PUBLISHER_ID);
	assert(pub_tid >= 0);
	tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	assert(tm_tid >= 0);


	tms.tmc = TM_SUBSCRIBE;

	while(true){
		Send(pub_tid, &tms, sizeof(tms), &res, sizeof(res));
		TMLogStrf(tm_tid, "Command: %d, Arg: %d\n", res.tc, res.arg1);
	}

	Exit();
}

void TrainProvider(){
	int reply = 0;

	int r = RegisterAs(TRAIN_PROVIDER_ID);
	assert(r == 0);

	tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	assert(tm_tid >= 0);
	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
	assert(tx_tid >= 0);

  	//Construct the Train Publisher
  	Create(29, &TrainPublisher);
  	// Create(19, &TestTMPublisher); //TODO: REMOVE THIS

	while(true){
		tid_t tid_req;
		TMProtocol tmp;
		TrainProtocol tp;

		Receive(&tid_req, &tmp, sizeof(tmp));

		switch(tmp.tmc){
			case TM_MOVE:
				Reply(tid_req, &reply, sizeof(reply));
				tp.tc = T_MOVE;
				tp.arg1 = tmp.arg1;
				tp.arg2 = tmp.arg2;
				CreateArgs(29, &TWriteTask, &tp, sizeof(tp));				
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}

	Exit();
}
