#include<stoppingcalibration_test.h>
#include <stop_at_test.h> //TODO: CHANGE THIS

int max(int i, int j) { return i < j ? j : i; };

void InitInch(InchingArgs args){
	int s_time, e1_time, e2_time, reply;
	//track_node *org = args.train->node;
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
	tm.tmc = TM_MOVE;
	tm.arg1 = args.train->id;
	tm.arg2 = 0;
	Send(args.tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
	// tm.tmc = TM_REVERSE;
	// tm.arg1 = args.train->id;
	// Send(args.tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
}

int InchForward(InchingArgs args){
	int s_time, e_time, reply;
	Destination next = GetNextSensorNI(args.switches, args.train->node);

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

	tm.tmc = TM_MOVE;
	tm.arg1 = args.train->id;
	tm.arg2 = 0;
	Send(args.tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	return (e_time - s_time) * args.train->inchingSpeed;
}

StopReference StopOverPath(TrainDescriptor *td, int stopDist){
	StopReference stopRef;
	int i, distance;
  path *p = &td->tpath;

	track_node *n, *next;
	i = p->ahead.size - 2;
  tr_path_get(&p->ahead, i, &n);

	distance = 0;
	do{
		if(i < 0){
			assert(0 && "Given path is too short!");
		}

    tr_path_get(&p->ahead, i, &n);
    tr_path_get(&p->ahead, i+1, &next);

    distance += track_node_dist(n, next);
		i--;
	}while(distance*1000 < stopDist || n->type != NODE_SENSOR);

	stopRef.ref = n;
  tr_path_get(&p->ahead, p->ahead.size-1, &stopRef.target);
	stopRef.delayDist = distance*1000 - stopDist;
	return stopRef;
}


void StoppingCalibrationTest(void * args){
	void *data;
	Destination dest;
	TrainDescriptor *td;
	int reply, distance;
	InchingArgs iargs;

	StoppingCalibrationArgs *scargs = (StoppingCalibrationArgs *)args;
	int train = scargs->train;
	int speed = scargs->speed;
	int befsensor = scargs->before_sensor;

	//Data of the grid
	track_node *track;
	Switch *switches;
	Sensor * sensors;
	TrainDescriptor *trains;

	track_node* target = NULL;
	track_node* before = NULL;

	tid_t my_tid = MyTid();
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
	//Update Start and Before
	before = &track[befsensor];

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

	//INITIALIZE
	td = &trains[train];
	iargs.tm_tid = tm_tid;
	iargs.sm_tid = sm_tid;
	iargs.cs_tid = cs_tid;
	iargs.my_tid = my_tid;
	iargs.switches = switches;
	iargs.train = td;

	//Guess initial delay (10ms)
	dest = GetNextSensorNI(switches, before);
	//END INITIALIZATION

	TMLogStrf(term_tid, "%d SPEED TEST\n", speed);
	Delay(cs_tid, my_tid, 30);

	//ready
	tm.tmc = TM_MOVE;
	tm.arg1 = train;
	tm.arg2 = speed;
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
	//Block until the "before" sensor
	sm.smr = SM_SUBSCRIBE;
	sm.byte = (char)before->num;
	Send(sm_tid, &sm, sizeof(sm), &reply, sizeof(reply));

	//Send Stop Command
	tm.tmc = TM_MOVE;
	tm.arg1 = train;
	tm.arg2 = 0;
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	//wait and determine target
	Delay(cs_tid, my_tid, speed*10*4);
	
	//Check if sitting on target
	if(sensors[td->node->num].state == SEN_ON){
		target = td->node;
		TMLogStrf(term_tid, "TARGET ACQUIRED %s\n", target->name);
		distance = DistanceBetweenNodes(switches, before, target)*1000;
		TMLogStrf(term_tid, "Distance: %d um\n", distance);
	}
	else{
		target = GetNextSensorNI(switches, td->node).node;
		TMLogStrf(term_tid, "TARGET ACQUIRED %s\n", target->name);
		distance = DistanceBetweenNodes(switches, before, target)*1000 - InchForward(iargs);
		TMLogStrf(term_tid, "Distance: %d um\n", distance);
	}

  /*
	//Dynamically test this
	StopAtProtocol sap;
  sap.sar = SAR_STOP;
  sap.train = train;
  sap.gear = speed;
  sap.stop_at = before->num;
  sap.dist = distance;

  tid_t sa_tid = WhoIs(STOP_AT_SERVER_ID);
  assert(sa_tid > 0);
  Send(sa_tid, &sap, sizeof(sap), &reply, sizeof(reply));

  TMLogStrf(term_tid, "Done!\n");
  */
	Exit();
}

void StoppingServerTest(){
	tid_t req_tid;
	int reply;
	StoppingCalibrationArgs args;

	int r = RegisterAs(STOPPING_CALIBRATION_ID);
	assert(r == 0);
  tid_t term_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(term_tid >= 0);

	while(true){
		Receive(&req_tid, &args, sizeof(args));
		Reply(req_tid, &reply, sizeof(reply));

		CreateArgs(20, StoppingCalibrationTest, (void *)&args);
	}
}

