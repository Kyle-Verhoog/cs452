#include <train_manager.h>

CIRCULAR_BUFFER_DEF(tc_cb, volatile TrainProtocol, TRAIN_COMMAND_BUFFER_SIZE);

void TMWriteTask(void *args){
	char buf[2];
	int reply;
	TrainDescriptor* td = (TrainDescriptor *)args;
	tid_t mytid = MyTid();
	tid_t parentTid = MyParentTid();
	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);

	volatile TrainProtocol cmd;
	tc_cb_pop(&td->buf, &cmd);

	switch(cmd.tc){
		case T_MOVE:
			buf[0] = cmd.arg2;
			buf[1] = cmd.arg1;
			PutStr(tx_tid, buf, 2);
			//Update the Descriptor
			td->speed = cmd.arg2;
			break;
		case T_REVERSE:
			buf[0] = 0;
	        buf[1] = cmd.arg1;
	        PutStr(tx_tid, buf, 2);
	        if(td->speed > 0){
	        	DelayCS(mytid, td->speed*10*2);	//Some arbitrary delay based on speed	
	        }	        
	        buf[0] = 15;
	        PutStr(tx_tid, buf, 2);
	        buf[0] = td->speed;
	        PutStr(tx_tid, buf, 2);
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

void TrainManager(){
	TrainDescriptor Trains[TRAIN_SIZE];
	int i;
	for(i = 0 ; i < TRAIN_SIZE; i++){
		INIT_TRAIN(Trains[i], i);
	}

	int reply = 0;
	int r = RegisterAs(TRAIN_MANAGER_ID);
  	assert(r == 0);
  	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);

	while(true){
		tid_t tid_req;
		TMProtocol tmp;
		TrainProtocol tp;

		Receive(&tid_req, &tmp, sizeof(tmp));
		Reply(tid_req, &reply, sizeof(reply));

		switch(tmp.tmc){
			case TM_MOVE:
				tp.tc = T_MOVE;
				tp.arg1 = tmp.arg1;
				tp.arg2 = tmp.arg2;
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
				if(Trains[(int)tmp.arg1].isRunning == -1){
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, (void *)&Trains[(int)tmp.arg1]);	
				}
				break;
			case TM_REVERSE:
				tp.tc = T_REVERSE;
				tp.arg1 = tmp.arg1;
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
        		if(Trains[(int)tmp.arg1].isRunning == -1){
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, (void *)&Trains[(int)tmp.arg1]);	
				}
				break;
			case TM_DELAY:
				tp.tc = T_DELAY;
				tp.arg1 = tmp.arg2;
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
        		if(Trains[(int)tmp.arg1].isRunning == -1){
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, (void *)&Trains[(int)tmp.arg1]);	
				}
				break;
			case TM_TASK_COMPLETE:
				if(Trains[(int)tmp.arg1].buf.size > 0){
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, (void *)&Trains[(int)tmp.arg1]);						
				}
				else{
					Trains[(int)tmp.arg1].isRunning = -1;	
				}
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}

	Exit();
}
