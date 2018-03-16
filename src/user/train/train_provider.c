#include <train_provider.h>
//#include <prediction_manager.h>
//#include <stoppingcalibration_test.h>

CIRCULAR_BUFFER_DEF(tc_cb, volatile TrainProtocol, TRAIN_COMMAND_BUFFER_SIZE);

void NotifyTMSubscribers(tid_cb *subscribers, TrainProtocol cmd){
	tid_t sub;
	while(tid_cb_pop(subscribers, &sub) != CB_E_EMPTY){
		Reply(sub, &cmd, sizeof(cmd));
	}
}

void TrainPublisher(){
	int reply;
	tid_t req_tid;
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
				NotifyTMSubscribers(&subscribers, tms.tp);
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

void TMWriteTask(void *args){
	char buf[2];
	int reply;
	volatile TrainProtocol cmd;
	TMSubscribe tms;
	TrainDescriptor* td = *(TrainDescriptor **)args;

	tid_t mytid = MyTid();
	tid_t parentTid = MyParentTid();
	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);
  	tid_t pub_tid = WhoIs(TRAIN_PUBLISHER_ID);
  	assert(pub_tid >= 0);

  	//Pop from instruction queue
	if(tc_cb_pop(&td->buf, &cmd) == CB_E_EMPTY){
		assert(0);
	}

	//Send the Updated Command to the publisher (TODO: Rare - Possible unwanted blocking)
	tms.tmc = TM_NOTIFY;
	tms.tp = cmd;
	Send(pub_tid, &tms, sizeof(tms), &reply, sizeof(reply));

	//Handle Command
	switch(cmd.tc){
		case T_MOVE:
			buf[0] = cmd.arg2;
			buf[1] = cmd.arg1;
			PutStr(tx_tid, buf, 2);
			//Update the Descriptor
			td->gear = cmd.arg2;
			break;
		case T_REVERSE:
			buf[0] = 0;
	        buf[1] = cmd.arg1;
	        PutStr(tx_tid, buf, 2);
	        if(td->gear > 0){
	        	DelayCS(mytid, td->gear*10*2);	//Some arbitrary delay based on speed
	        }
	        buf[0] = 15;
	        PutStr(tx_tid, buf, 2);
	        buf[0] = td->gear;
	        PutStr(tx_tid, buf, 2);
	        td->dir = td->dir ? false:true;
	        if(td->node != NULL){
	        	td->node = td->node->reverse;	
	        }	        
			break;
		case T_DELAY:
			DelayCS(mytid, cmd.arg1);
			break;
		default:
			assert(0 && "Bad Train Command");
			break;
	}

	//Send to Train Manager to complete the task
	TMProtocol tmp;
	tmp.tmc = TM_TASK_COMPLETE;
	tmp.arg1 = td->id;
	Send(parentTid, &tmp, sizeof(TMProtocol), &reply, sizeof(reply));

	Exit();
}
void init_train_model(TrainDescriptor *td, int train_id){
	td->id = train_id; 
	td->gear = 0; 
	td->speed = 0; 
	td->dir = 1; 
	td->exist = false; 
	td->isRunning = -1; 
	td->node = NULL; 
  path_init(&td->tpath);
	tc_cb_init(&td->buf);
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
	void *data;
	TrainDescriptor Trains[TRAIN_SIZE];
	
  int r = RegisterAs(TRAIN_PROVIDER_ID);
  assert(r == 0);
	int i;
	for(i = 0 ; i < TRAIN_SIZE; i++){
		init_train_model(&Trains[i], i);
	}

	int reply = 0;
  	tid_t mytid = MyTid();
  	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
  	assert(cs_tid >= 0);
  	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);

  	//Construct the Train Publisher
  	Create(29, &TrainPublisher);

	while(true){
		tid_t tid_req;
		TMProtocol tmp;
		TrainProtocol tp;

		Receive(&tid_req, &tmp, sizeof(tmp));

    TrainDescriptor *td;
		switch(tmp.tmc){
			case TM_MOVE:
				Reply(tid_req, &reply, sizeof(reply));
				tp.tc = T_MOVE;
				tp.arg1 = tmp.arg1;
				tp.arg2 = tmp.arg2;
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
				if(Trains[(int)tmp.arg1].isRunning == -1){
          td = &Trains[(int)tmp.arg1];
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, &td, sizeof(TrainDescriptor *));
				}
				break;
			case TM_REVERSE:
				Reply(tid_req, &reply, sizeof(reply));
				tp.tc = T_REVERSE;
				tp.arg1 = tmp.arg1;
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
        if(Trains[(int)tmp.arg1].isRunning == -1){
          td = &Trains[(int)tmp.arg1];
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, &td, sizeof(TrainDescriptor *));
        }
				break;
			case TM_DELAY:
				Reply(tid_req, &reply, sizeof(reply));
				tp.tc = T_DELAY;
				tp.arg1 = tmp.arg2;
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
        if(Trains[(int)tmp.arg1].isRunning == -1){
          td = &Trains[(int)tmp.arg1];
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, &td, sizeof(TrainDescriptor *));
        }
				break;
			case TM_TASK_COMPLETE:
				Reply(tid_req, &reply, sizeof(reply));
				if(Trains[(int)tmp.arg1].buf.size > 0){
          td = &Trains[(int)tmp.arg1];
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, &td, sizeof(TrainDescriptor *));
				}
				else{
					Trains[(int)tmp.arg1].isRunning = -1;	
				}
				break;
			case TM_TRACK:
				Trains[(int)tmp.arg1].node = &track[(int)tmp.arg2];
				Trains[(int)tmp.arg1].exist = true;
				Trains[(int)tmp.arg1].time_of_sensor = Time(cs_tid, mytid);
				Reply(tid_req, &reply, sizeof(reply));
				break;
			case TM_GET_ALL:
				data = (void *)Trains;
				Reply(tid_req, &data, sizeof(data));
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}

	Exit();
}
