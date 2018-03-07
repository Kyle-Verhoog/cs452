#include <stop_at_test.h>

void InitInch(InchingArgs args){
	int s_time, e1_time, e2_time, reply;
	track_node *org = args.train->node;
	Destination next = GetNextSensorNI(args.switches, args.train->node);
	Destination next2 = GetNextSensorNI(args.switches, next.node);

	tid_t term_tid = WhoIs(TERMINAL_MANAGER_ID);
  	assert(term_tid >= 0);

	TMProtocol tm;
	SMProtocol sm;

	//Move the Train
	tm.tmc = TM_MOVE;
	tm.arg1 = args.train->id;
	tm.arg2 = INCHING_GEAR;
	Send(args.tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	s_time = Time(args.cs_tid, args.my_tid);

	//Block until the "next" sensor
	sm.smr = SM_SUBSCRIBE;
	sm.byte = (char)next.node->num;
	Send(args.sm_tid, &sm, sizeof(sm), &reply, sizeof(reply));

	e1_time = Time(args.cs_tid, args.my_tid);

	//Block until the "next2" sensor
	sm.smr = SM_SUBSCRIBE;
	sm.byte = (char)next2.node->num;
	Send(args.sm_tid, &sm, sizeof(sm), &reply, sizeof(reply));

	e2_time = Time(args.cs_tid, args.my_tid);

	//Determine Speed
	args.train->inchingSpeed = (next2.dist*1000)/(e2_time - e1_time);
	assert(args.train->inchingSpeed > 0);

	TMLogStrf(term_tid, "Speed: %d\n", args.train->inchingSpeed);

	//Reverse back to approx same location
	// tm.tmc = TM_REVERSE;
	// tm.arg1 = args.train->id;
	// Send(args.tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
	// //Wait until we're back
	// sm.smr = SM_SUBSCRIBE;
	// sm.byte = (char)org->num;
	// Send(args.sm_tid, &sm, sizeof(sm), &reply, sizeof(reply));

	// //Set Speed to be 0 and reverse back
	// tm.tmc = TM_MOVE;
	// tm.arg1 = args.train->id;
	// tm.arg2 = 0;
	// Send(args.tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
	// tm.tmc = TM_REVERSE;
	// tm.arg1 = args.train->id;
	// Send(args.tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
}

int inchForward(InchingArgs args){
	int s_time, e_time, reply;
	Destination next = GetNextSensor(args.switches, args.train->node);

	TMProtocol tm;
	SMProtocol sm;

	//Move the Train
	tm.tmc = TM_MOVE;
	tm.arg1 = args.train->id;
	tm.arg2 = INCHING_GEAR;
	Send(args.tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	s_time = Time(args.cs_tid, args.my_tid);

	//Block until the "next" sensor
	sm.smr = SM_SUBSCRIBE;
	sm.byte = (char)next.node->num;
	Send(args.sm_tid, &sm, sizeof(sm), &reply, sizeof(reply));

	e_time = Time(args.cs_tid, args.my_tid);

	return (e_time - s_time) * args.train->inchingSpeed;
}

void StopAtServer(){
	int reply;
	tid_t req_tid;
	StopAtProtocol sap;

	int r = RegisterAs(STOP_AT_SERVER_ID);
	assert(r == 0);

	while(true){
		Receive(&req_tid, &sap, sizeof(sap));
		Reply(req_tid, &reply, sizeof(reply));

		switch(sap.sar){
			case SAR_STOP:
				CreateArgs(20, &StopAt, (void*)&sap);
				break;
			default:
				assert(0 && "Bad Command");
		}
	}
}

void StopAt(void *args){
	void *data;
	int stoppingDistances[15] = {0, 0, 0, 0, 50, 688, 40920, 81854, 163424, 283460, 362040, 520000, 737960, 937387, 1294021};
	StopAtProtocol stopAt = *(StopAtProtocol *)args;

	//Data of the grid
	track_node *track;
	TrainDescriptor *trains;
	Switch *switches;
	Sensor * sensors;

	Destination target;
	TrainDescriptor *train;
	int stoppingDistance, delay, reply;

	tid_t mytid = MyTid();
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
	Send(tm_tid, &tm, sizeof(tm), &trains, sizeof(trains));

	//Get Switch List
	sw.swr = SW_GET_ALL;
	Send(sw_tid, &sw, sizeof(sw), &switches, sizeof(switches));

	//Get Sensor List
	sm.smr = SM_GET_ALL;
	Send(sm_tid, &sm, sizeof(sm), &sensors, sizeof(sensors));

	//Initialize
	train = &trains[stopAt.train];
	target.node = &track[stopAt.stop_at];
	target.dist = DistanceBetweenNodes(switches, &track[stopAt.stop_from], target.node); //TODO: MAKE STARTING DYNAMIC
	stoppingDistance = stoppingDistances[stopAt.gear];	//TODO: DETERMINE BASED ON (TRAIN,SPEED)

	// sw.swr = SW_FLIP;
	// sw.sw = 14;
	// sw.dir = SW_STRAIGHT;
	// Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));

	// sw.sw = 17;
	// sw.dir = SW_STRAIGHT;
	// Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));

	//Send the TK
	tm.tmc = TM_TRACK;
	tm.arg1 = stopAt.train;
	tm.arg2 = stopAt.start;
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	TMRegister(term_tid, CAL_OFFSET_X, CAL_OFFSET_Y, CAL_WIDTH, CAL_HEIGHT);
	TMLogStrf(term_tid, "st: %d, %d, %d, %d, %d\n", stopAt.train, stopAt.gear, stopAt.stop_at, stopAt.stop_from, stopAt.start);
	//Add Initial Delay
	Delay(cs_tid, mytid, 30);	

	//Move the Train
	tm.tmc = TM_MOVE;
	tm.arg1 = stopAt.train;
	tm.arg2 = stopAt.gear;
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	//TEST
	//Block until the "start_from" sensor
	sm.smr = SM_SUBSCRIBE;
	sm.byte = (char)stopAt.stop_from;
	Send(sm_tid, &sm, sizeof(sm), &reply, sizeof(reply));
	delay = (target.dist*1000 - stoppingDistance)/train->speed;
	TMLogStrf(term_tid, "Delay: %d, %d, %d, %d\n", delay, target.dist*1000, stoppingDistance, train->speed);
	//assert(delay >= 50);

	//Wait until actual position before sending stop
	if(delay){
		Delay(cs_tid, mytid, delay);	
	}

	//Send Stop
	tm.tmc = TM_MOVE;
	tm.arg1 = stopAt.train;
	tm.arg2 = 0;
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	//Delay
	Delay(cs_tid, mytid, stopAt.gear*10*4);

	//Check if sitting on target
	if(sensors[target.node->num].state == SEN_ON){
		TMLogStrf(term_tid, "Stop time: %d ticks\n", delay);
	}
	else{
		Destination over = GetPrevSensor(switches, train->node);
		if(over.node != target.node){
			TMLogStrf(term_tid, "Undershot: %d ticks\n", delay);
		}else{
			TMLogStrf(term_tid, "Overshot: %d ticks\n", delay);
		}
	}	
	
	Exit();
}
