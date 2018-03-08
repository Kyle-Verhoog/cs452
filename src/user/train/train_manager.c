#include <train_manager.h>
#include <prediction_manager.h>
#include <stoppingcalibration_test.h>

CIRCULAR_BUFFER_DEF(tc_cb, volatile TrainProtocol, TRAIN_COMMAND_BUFFER_SIZE);

void AddTrainToPrediction(tid_t pred_tid, TrainDescriptor *train){
	int reply;
	PMProtocol pmp;
	pmp.pmc = PM_TRAIN;
	pmp.args = (void *)train;
	pmp.size = 1;
	
	Send(pred_tid, &pmp, sizeof(pmp), &reply, sizeof(reply));
}

void MeasureTrainForPredition(tid_t pred_tid, TrainDescriptor *train){
	int reply;
	PMProtocol pmp;
	pmp.pmc = PM_MEASURE;
	pmp.args = (void *)train;
	pmp.size = 1;
	
	Send(pred_tid, &pmp, sizeof(pmp), &reply, sizeof(reply));
}

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
	        td->dir = !td->dir;
	        td->node = td->node->reverse;
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
	tc_cb_init(&td->buf);
}

void TrainManager(void *args){
	void *data;
	track_node *track = (track_node *)args;
	TrainDescriptor Trains[TRAIN_SIZE];
	int i;
	for(i = 0 ; i < TRAIN_SIZE; i++){
		init_train_model(&Trains[i], i);
	}

	int reply = 0;
	int r = RegisterAs(TRAIN_MANAGER_ID);
  	assert(r == 0);
  	tid_t mytid = MyTid();
  	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
  	assert(cs_tid >= 0);
  	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	assert(tx_tid >= 0);
  	tid_t pred_tid = WhoIs(PREDICTION_MANAGER_ID);
  	assert(pred_tid >= 0);

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
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
				if(Trains[(int)tmp.arg1].isRunning == -1){
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, (void *)&Trains[(int)tmp.arg1]);	
				}
				break;
			case TM_REVERSE:
				Reply(tid_req, &reply, sizeof(reply));
				tp.tc = T_REVERSE;
				tp.arg1 = tmp.arg1;
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
        		if(Trains[(int)tmp.arg1].isRunning == -1){
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, (void *)&Trains[(int)tmp.arg1]);	
				}
				break;
			case TM_DELAY:
				Reply(tid_req, &reply, sizeof(reply));
				tp.tc = T_DELAY;
				tp.arg1 = tmp.arg2;
				tc_cb_push(&Trains[(int)tmp.arg1].buf, tp);
        		if(Trains[(int)tmp.arg1].isRunning == -1){
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, (void *)&Trains[(int)tmp.arg1]);	
				}
				break;
			case TM_TASK_COMPLETE:
				Reply(tid_req, &reply, sizeof(reply));
				if(Trains[(int)tmp.arg1].buf.size > 0){
					Trains[(int)tmp.arg1].isRunning = CreateArgs(29, &TMWriteTask, (void *)&Trains[(int)tmp.arg1]);						
				}
				else{
					Trains[(int)tmp.arg1].isRunning = -1;	
				}
				break;
			case TM_TRACK:
				Reply(tid_req, &reply, sizeof(reply));
				Trains[(int)tmp.arg1].node = &track[(int)tmp.arg2];
				Trains[(int)tmp.arg1].exist = true;
				Trains[(int)tmp.arg1].time_of_sensor = Time(cs_tid, mytid);
				AddTrainToPrediction(pred_tid, &Trains[(int)tmp.arg1]);
				break;
			case TM_MEASURE:
				Reply(tid_req, &reply, sizeof(reply));
				MeasureTrainForPredition(pred_tid, &Trains[(int)tmp.arg1]);
				break;
			case TM_GET_ALL:
				data = (void *)Trains;
				Reply(tid_req, &data, sizeof(void *));
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}

	Exit();
}
