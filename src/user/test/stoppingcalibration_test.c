#include<stoppingcalibration_test.h>
#include <stop_at_test.h> //TODO: CHANGE THIS

static tid_t term_tid;

int max(int i, int j) { return i < j ? j : i; };

void StoppingCalibrationTest(void * args){
	void *data;
	Destination dest;
	TrainDescriptor *td;
	int reply, delay, currentSpeed, distance;
	int fraction = 10;
	
	StoppingCalibrationArgs *scargs = (StoppingCalibrationArgs *)args;
	int train = scargs->train;
	int speed = scargs->speed;
	int befsensor = scargs->before_sensor;
	int start_node = scargs->start_node;

	//Data of the grid
	track_node *track;
	Switch *switches;
	Sensor * sensors;
	TrainDescriptor *trains;

	track_node* target = NULL;
	track_node* before = NULL;

	tid_t mytid = MyTid();
	tid_t parentTid = MyParentTid();
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
	Send(tm_tid, &tm, sizeof(tm), &trains, sizeof(trains));

	//Get Switch List
	sw.swr = SW_GET_ALL;
	Send(sw_tid, &sw, sizeof(sw), &switches, sizeof(switches));

	//Get Sensor List
	sm.smr = SM_GET_ALL;
	Send(sm_tid, &sm, sizeof(sm), &sensors, sizeof(sensors));
	

	//INITIALIZE
	td = &trains[train];

	// sw.swr = SW_FLIP;
	// sw.sw = 14;
	// sw.dir = SW_STRAIGHT;
	// Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));

	// sw.sw = 17;
	// sw.dir = SW_STRAIGHT;
	// Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));

	// Delay(cs_tid, mytid, 200);

	//Send the TK
	tm.tmc = TM_TRACK;
	tm.arg1 = train;
	tm.arg2 = start_node;
	Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

	//Guess initial delay (10ms)
	dest = GetNextSensorNI(switches, before);
	//END INITIALIZATION

	TMLogStrf(term_tid, "%d SPEED TEST From: %s\n", speed, track[start_node].name);
	Delay(cs_tid, mytid, 30);

	//ready
	while(true){
		tm.tmc = TM_MOVE;
		tm.arg1 = train;
		tm.arg2 = speed;
		Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));
		//Block until the "before" sensor
		sm.smr = SM_SUBSCRIBE;
		sm.byte = (char)before->num;
		Send(sm_tid, &sm, sizeof(sm), &reply, sizeof(reply));
		delay = (fraction*dest.dist*1000)/(td->speed * 10);
		currentSpeed = td->speed;
		TMLogStrf(term_tid, "Stopping (%d)\n", delay);
		//Delay some more
		Delay(cs_tid, mytid, delay);

		//Send Stop Command
		tm.tmc = TM_MOVE;
		tm.arg1 = train;
		tm.arg2 = 0;
		Send(tm_tid, &tm, sizeof(tm), &reply, sizeof(reply));

		if(target == NULL){
			//wait and determine target
			Delay(cs_tid, mytid, speed*10*4);
			target = GetPrevSensor(switches, td->node).node;
			TMLogStrf(term_tid, "TARGET ACQUIRED %s\n", target->name);
		}
		else{
			//Delay
			Delay(cs_tid, mytid, speed*10*4);	
		}
		
		//Check if sitting on target
		if(sensors[target->num].state == SEN_ON){
			distance = DistanceBetweenNodes(switches, before, target)*1000;
			distance -= delay * currentSpeed;
			TMLogStrf(term_tid, "Stop time: %d ticks\n", delay);
			TMLogStrf(term_tid, "Distance: %d um\n", distance);
			break;
		}
		else{
			track_node *over = GetPrevSensor(switches, td->node).node;
			if(over != target){
				distance = DistanceBetweenNodes(switches, before, target)*1000;
				distance -= delay * currentSpeed;
				TMLogStrf(term_tid, "Undershot: %d ticks\n", delay);
				TMLogStrf(term_tid, "Distance: %d um\n", distance);
				break;
			}
		}

		//Subtract delay by some epsilon
		fraction--;
	}

	int targetNum = target->num;
	Send(parentTid, &targetNum, sizeof(targetNum), &reply, sizeof(reply));
	Exit();
}

void StoppingServerTest(){
	tid_t req_tid;
	int reply;
	StoppingCalibrationArgs args;
	args.speed = 7;
	args.train = 24;
	args.before_sensor = 60; //D13
	args.start_node = 0;	//A1

  term_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(term_tid >= 0);

  TMRegister(term_tid, CAL_OFFSET_X, CAL_OFFSET_Y, CAL_WIDTH, CAL_HEIGHT);

	while(true){
		if(args.speed < 4){
			break;
		}

		CreateArgs(20, StoppingCalibrationTest, (void *)&args);
		Receive(&req_tid, &reply, sizeof(reply));
		Reply(req_tid, &reply, sizeof(reply));

		args.start_node = reply;
		args.speed--;
	}
}
