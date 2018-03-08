#include <stop_at_test.h>

void StopAtServer(){
	int reply;
	tid_t req_tid;
	StopAtProtocol sap;

	int r = RegisterAs(STOP_AT_SERVER_ID);
	assert(r == 0);

	while(true){
		Receive(&req_tid, &sap, sizeof(sap));

		switch(sap.sar){
			case SAR_STOP:
				CreateArgs(20, &StopAt, (void*)&sap);
				break;
			default:
				assert(0 && "Bad Command");
		}		
		Reply(req_tid, &reply, sizeof(reply));
	}
}

void StopAt(void *args){
	void *data;
	int delay, reply;
	int stoppingDistances[15] = {0, 0, 0, 50, 888, 16252, 44286, 100088, 137681, 267966, 388076, 543877, 811169, 973337, 1255858};
	StopAtProtocol* stopAt = (StopAtProtocol *)args;
	int trainId = stopAt->train;
	int gear = stopAt->gear;
	int stop_at = stopAt->stop_at;
	int dist = stopAt->dist;

	//Data of the grid
	track_node *track;
	TrainDescriptor *trains;
	Switch *switches;
	Sensor * sensors;

	track_node *target;
	TrainDescriptor *train;
	StopReference stopRef;

	tid_t mytid = MyTid();
	//tid_t parent_tid = MyParentTid();
	tid_t term_tid = WhoIs(TERMINAL_MANAGER_ID);
  	assert(term_tid >= 0);
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	assert(cs_tid >= 0);
	tid_t rw_tid = WhoIs(RAILWAY_MANAGER_ID);
	assert(rw_tid >= 0);
	tid_t tm_tid = WhoIs(TRAIN_MANAGER_ID);
	assert(tm_tid >= 0);
	tid_t sw_tid = WhoIs(SWITCH_MANAGER_ID);
	assert(sw_tid >= 0);
	tid_t sm_tid = WhoIs(SENSOR_MANAGER_ID);
	assert(sm_tid >= 0);

	RWProtocol rw;
	TMProtocol tm;
	SWProtocol sw;
	SMProtocol sm;

	//Get track data
	rw.rwc = RW_GET_TRACK;
	Send(rw_tid, &rw, sizeof(rw), &data, sizeof(data));
	track = (track_node *)data;

	//Get Train List
	tm.tmc = TM_GET_ALL;
	Send(tm_tid, &tm, sizeof(tm), &data, sizeof(data));
	trains = (TrainDescriptor *)data;

	//Get Switch List
	sw.swr = SW_GET_ALL;
	Send(sw_tid, &sw, sizeof(sw), &data, sizeof(data));
	switches = (Switch *)data;

	//Get Sensor List
	sm.smr = SM_GET_ALL;
	Send(sm_tid, &sm, sizeof(sm), &data, sizeof(data));
	sensors = (Sensor *)data;

	//Initialize
	train = &trains[trainId];
	target = &track[stop_at];

	//If distance exists, update
	if(dist != -1){
		stoppingDistances[gear] = dist;
	}

	stopRef = StopOverPath(train, stoppingDistances[gear]);

	//Move the Train
	tm.tmc = TM_MOVE;
	tm.arg1 = trainId;
	tm.arg2 = gear;
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	//TEST
	TMLogStrf(term_tid, "Train target: %s\n", stopRef.target->name);
	TMLogStrf(term_tid, "Train blocking until: %s\n", stopRef.ref->name);
	//Block until the "ref" sensor
	sm.smr = SM_SUBSCRIBE;
	sm.byte = (char)stopRef.ref->num;
	Send(sm_tid, &sm, sizeof(sm), &reply, sizeof(reply));

	//Delay after ref
	delay = stopRef.delayDist/(train->speed);
	assert(delay >= 0);

	//Wait until actual position before sending stop
	TMLogStrf(term_tid, "Train Stopping Delay: %d ticks\n", delay);
	if(delay){
		Delay(cs_tid, mytid, delay);	
	}

	//Send Stop
	tm.tmc = TM_MOVE;
	tm.arg1 = trainId;
	tm.arg2 = 0;
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	//Delay
  assert(gear*10*4 > 0);
	Delay(cs_tid, mytid, gear*10*4);

	//Check if sitting on target
	if(sensors[target->num].state == SEN_ON){
		TMLogStrf(term_tid, "Stop time: %d ticks\n", delay);
	}
	else{
		Destination over = GetPrevSensor(switches, train->node);
		if(over.node != target){
			TMLogStrf(term_tid, "Undershot: %d ticks\n", delay);
		}else{
			TMLogStrf(term_tid, "Overshot: %d ticks\n", delay);
		}
	}

	Exit();
}
