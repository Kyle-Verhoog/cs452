#include <user/train/calibration.h>

int measuring_velocity; //Hack-y

void TrainSpeedSubscriber(void * args){
	int train, r;
	tid_t rep_tid, up_tid;
	TrackRequest tr_req;
	TETRSpeed event;

	train = *(int *)args;

	rep_tid = WhoIs(REPRESENTER_ID);
	up_tid = MyParentTid();
	assert(rep_tid > 0 && up_tid > 0);

	//Init Request
	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_SPEED;

	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.num == train){
			Send(up_tid, &event.new, sizeof(event.new), &r, sizeof(r));
			if(r == -1){
				break;
			}
		}		
	}

	Exit();
}

void TrainSpeedUpdate(void *args){
	int train, r, speed, command;
	tid_t tc_tid, req_tid;
	bool canExit = false;

	train = *(int *)args;

	tc_tid = MyParentTid();
	CreateArgs(19, &TrainSpeedSubscriber, (void*)&train, sizeof(train));
	r = 0;
	speed = 0;

	while(!canExit){
		Receive(&req_tid, &command, sizeof(command));
		
		switch(command){
			case 0:
				r = -1;
				Reply(req_tid, &speed, sizeof(speed));
				break;
			case 1:
				Reply(req_tid, &speed, sizeof(speed));		
				break;
			default:
				speed = command;
				Reply(req_tid, &r, sizeof(r));
				if(r == -1){
					canExit = true;
				}
		}

	}

	Exit();
}

void Calibration(void *args){
	int r, stime, etime, dist;
	CalibrationArgs cargs;
	TrackRequest tr_req;
	tid_t cs_tid, tm_tid, rep_tid, tr_tid, my_tid;
	TETRPosition event;
	TrainProtocol tp;
	track_node *start, *end;
	Switch switches[SWITCH_SIZE];

	cargs = *(CalibrationArgs *)args;

	my_tid = MyTid();
	cs_tid = WhoIs(CLOCKSERVER_ID);
	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	rep_tid = WhoIs(REPRESENTER_ID);
	tr_tid = WhoIs(TRAIN_PROVIDER_ID);
	assert(tm_tid > 0 && cs_tid > 0 && rep_tid > 0 && tr_tid > 0);

	//Initialize subscription
	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_POSITION;

	//Send Speed
	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = cargs.init_gear;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = 0;

	//Wait until starting point
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node->num == cargs.target_node &&
			event.num == cargs.train){
			break;
		}	
	}

	//Send stop
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Wait for train to stop
	Delay(cs_tid, my_tid, 10*3*cargs.init_gear);	//Some arbitrary delay to ensure

	//Send Measurement Speed
	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = MEASURING_GEAR;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Calculate time
	stime = Time(cs_tid, my_tid);

	//Wait Reach Next Sensor Destination
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node->type == NODE_SENSOR &&
			event.num == cargs.train){
			break;
		}	
	}

	//Calculate time
	etime = Time(cs_tid, my_tid);

	//Send Stop
	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = 0;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Get Switch Configuration
	tr_req.type = TRR_FETCH;
	tr_req.data.dtype = TD_SW;
	Send(rep_tid, &tr_req, sizeof(tr_req), &switches, sizeof(switches));

	//DO SOME CALCULATION TO DETERMINE TOTAL DISTANCE HERE
	//NEED THE SWITCH STATE AT THIS POINT
	start = &TRACK[cargs.target_node];
	end = event.node;
	dist = dist_between_nodes(switches, start, end)*1000 - (etime - stime)*measuring_velocity;

	//Log
	TMLogStrf(tm_tid, "Tr: %d, Gear: %d, Dist: %d\n", cargs.train, cargs.init_gear, dist);

	Exit();
}

void AccelCalibration(void *args){
	int r, stime, etime, dist, accelRun, veloRun, velocity, accelDist;
	CalibrationArgs cargs;
	TestCalibArgs tcargs;
	TrackRequest tr_req;
	tid_t cs_tid, tm_tid, rep_tid, tr_tid, my_tid, spd_tid;
	TETRPosition event;
	TrainProtocol tp;
	track_node *target_node;
	Switch switches[SWITCH_SIZE];
	PossibleSensor pos;

	cargs = *(CalibrationArgs *)args;
	spd_tid = CreateArgs(19, &TrainSpeedUpdate, (void *)&cargs.train, sizeof(cargs.train));
	my_tid = MyTid();
	cs_tid = WhoIs(CLOCKSERVER_ID);
	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	rep_tid = WhoIs(REPRESENTER_ID);
	tr_tid = WhoIs(TRAIN_PROVIDER_ID);
	assert(tm_tid > 0 && cs_tid > 0 && rep_tid > 0 && tr_tid > 0);

	//Get Switch Configuration
	tr_req.type = TRR_FETCH;
	tr_req.data.dtype = TD_SW;
	Send(rep_tid, &tr_req, sizeof(tr_req), &switches, sizeof(switches));

	//Set target node
	target_node = &TRACK[cargs.target_node];

	//Initialize subscription
	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_POSITION;

	//Move train until starting location
	//tcargs.train = cargs.train;
	//tcargs.gear = MEASURING_GEAR;
	//tcargs.dist = 0;
	//tcargs.target_node = cargs.target_node;

	//CreateArgs(19, &TestCalibration, (void*)&tcargs, sizeof(tcargs));
	//while(true){
	//	Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
	//	if(event.node == target_node &&
	//		event.num == cargs.train){
	//		break;
	//	}
	//}

	TMLogStrf(tm_tid, "Got Here %s\n",target_node->name);

	//Calculating travel distance needed
	dist = 0;
	pos.node = target_node;
	pos.dist = target_node->edge[DIR_AHEAD].dist;
	do{
		dist += pos.dist;
		GetNextSensorEXC(switches, pos.node, &pos);
	}while(pos.node != target_node);

	//Set train up to speed
	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = cargs.init_gear;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Start time
	stime = Time(cs_tid, my_tid);

	//Wait until sensor has been triggered again
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node == target_node &&
			event.num == cargs.train){
			break;
		}	
	}

	//Ending time
	etime = Time(cs_tid, my_tid);
	accelRun = etime - stime;
	stime = etime;	//Reset and run again

	//Wait until sensor has been triggered again
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node == target_node &&
			event.num == cargs.train){
			break;
		}	
	}	

	//Ending time
	etime = Time(cs_tid, my_tid);
	veloRun = etime - stime;
	assert(accelRun > veloRun);
	
	//Get Velocity
	r = 0;
	Send(spd_tid, &r, sizeof(r), &velocity, sizeof(velocity));

	//Stop Train
	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = 0;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//aceleration distance (Max acceleration dist)
	accelDist = (velocity*(accelRun-veloRun));

	//Log Data
	TMLogStrf(tm_tid, "Accel Time: %d, Dist: %d\n", accelRun - veloRun, accelDist);
	Exit();
}

void SetMS(void *args){
	tid_t tm_tid;

	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	assert(tm_tid > 0);

	measuring_velocity = *(int *)args;
	TMLogStrf(tm_tid, "Train MS set to %d\n", measuring_velocity);
	Exit();
}

void MeasuringVelocity(void *args){
	int r, stime, etime, dist, train;
	TrackRequest tr_req;
	tid_t cs_tid, tm_tid, rep_tid, tr_tid, my_tid;
	TETRPosition event;
	track_node *start, *end;
	TrainProtocol tp;
	Switch switches[SWITCH_SIZE];
	
	train = *(int *)args;

	my_tid = MyTid();
	cs_tid = WhoIs(CLOCKSERVER_ID);
	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	rep_tid = WhoIs(REPRESENTER_ID);
	tr_tid = WhoIs(TRAIN_PROVIDER_ID);
	assert(tm_tid > 0 && cs_tid > 0 && rep_tid > 0 && tr_tid > 0);

	//Initialize subscription
	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_POSITION;

	tp.tc = T_MOVE;
	tp.arg1 = train;
	tp.arg2 = MEASURING_GEAR;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));	

	//Wait until starting point
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node->type == NODE_SENSOR &&
			event.num == train){
			break;
		}	
	}

	stime = Time(cs_tid, my_tid);
	start = event.node;

	//Wait until end point
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node->type == NODE_SENSOR &&
			event.num == train){
			break;
		}	
	}

	etime = Time(cs_tid, my_tid);
	end = event.node;

	//Send Stop
	tp.tc = T_MOVE;
	tp.arg1 = train;
	tp.arg2 = 0;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));	

	//Get Switch Configuration
	tr_req.type = TRR_FETCH;
	tr_req.data.dtype = TD_SW;
	Send(rep_tid, &tr_req, sizeof(tr_req), &switches, sizeof(switches));

	//COMPUTE THE VELOCITY
	dist = dist_between_nodes(switches, start, end)*1000;
	measuring_velocity = dist/(etime - stime);

	//Log
	TMLogStrf(tm_tid, "Measuring Velocity: %d\n", measuring_velocity);
	Exit();
}

static int GetLastAvailableSensor(track_node *start, track_node *end, Switch *sw, int min_dist, PossibleSensor *target){
	PossibleSensor pos;
	track_node *n;
	int dist, r;

  pos.dist = 0;
	target->dist = 0;
	pos.node = start;
	dist = dist_between_nodes(sw, start, end);
	if(dist*1000 < min_dist){
		return -1;
	}

	while(true){
		n = pos.node;
		r = GetNextSensorEXC(sw, n, &pos);
		assert(r == 0);		

		if((dist - pos.dist)*1000 < min_dist){
			break;
		}

		dist -= pos.dist;
		target->dist += pos.dist;
	}

	target->node = n;
	return dist;
}

void TestCalibration(void *args){
	int r, dist, delayDist, velocity;
	TestCalibArgs tcargs;
	TrackRequest tr_req;
	tid_t cs_tid, tm_tid, rep_tid, tr_tid, my_tid, spd_tid;
	TETRPosition event;
	track_node *start, *end;
	PossibleSensor target;
  target.node = NULL;
	TrainProtocol tp;
	Switch switches[SWITCH_SIZE];

	tcargs = *(TestCalibArgs *)args;

	my_tid = MyTid();
	cs_tid = WhoIs(CLOCKSERVER_ID);
	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	rep_tid = WhoIs(REPRESENTER_ID);
	tr_tid = WhoIs(TRAIN_PROVIDER_ID);
	assert(tm_tid > 0 && cs_tid > 0 && rep_tid > 0 && tr_tid > 0);

	spd_tid = CreateArgs(19, &TrainSpeedUpdate, (void *)&tcargs.train, sizeof(tcargs.train));

	//Get Switch Configuration
	tr_req.type = TRR_FETCH;
	tr_req.data.dtype = TD_SW;
	Send(rep_tid, &tr_req, sizeof(tr_req), &switches, sizeof(switches));

	tp.tc = T_MOVE;
	tp.arg1 = tcargs.train;
	tp.arg2 = tcargs.gear;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Initialize subscription
	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_POSITION;

	//Get Current Position
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node->type == NODE_SENSOR &&
			event.num == tcargs.train){
			break;
		}	
	}

	//Determine Start & End
	start = event.node;
	end = &TRACK[tcargs.target_node];

	//Decide which is the last sensor  (returns dist from end)
	dist = GetLastAvailableSensor(start, end, switches, tcargs.dist, &target);
	assert(dist >= 0);

	TMLogStrf(tm_tid, "Last Sensor to Stop: %s\n", target.node->name);

	//Wait for the last possible sensor
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node->num == target.node->num &&
			event.num == tcargs.train){
			break;
		}	
	}

	//Delay the amount needed (NEED TRAIN VELOCITY)
	delayDist = dist*1000 - tcargs.dist;

	//Get Velocity
	r = 0;
	Send(spd_tid, &r, sizeof(r), &velocity, sizeof(velocity));

	//Delay given time
	Delay(cs_tid, my_tid, delayDist/velocity);

	//Send Stop
	tp.tc = T_MOVE;
	tp.arg1 = tcargs.train;
	tp.arg2 = 0;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));
	Exit();
}

void ATestCalibration(void *args){
	int r, dist, stime, etime;
	ATestCalibArgs tcargs;
	TrackRequest tr_req;
	tid_t cs_tid, tm_tid, rep_tid, tr_tid, my_tid;
	TETRPosition event;
	track_node *start, *end;
	PossibleSensor target;
	TrainProtocol tp;
	Switch switches[SWITCH_SIZE];

	tcargs = *(ATestCalibArgs *)args;

	my_tid = MyTid();
	cs_tid = WhoIs(CLOCKSERVER_ID);
	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	rep_tid = WhoIs(REPRESENTER_ID);
	tr_tid = WhoIs(TRAIN_PROVIDER_ID);
	assert(tm_tid > 0 && cs_tid > 0 && rep_tid > 0 && tr_tid > 0);

	//Get Switch Configuration
	tr_req.type = TRR_FETCH;
	tr_req.data.dtype = TD_SW;
	Send(rep_tid, &tr_req, sizeof(tr_req), &switches, sizeof(switches));

	//Initialize subscription
	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_POSITION;

	//Move Train
	tp.tc = T_MOVE;
	tp.arg1 = tcargs.train;
	tp.arg2 = MEASURING_GEAR;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Wait for an up coming sensor
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node->type == NODE_SENSOR &&
			event.num == tcargs.train){
			break;
		}	
	}

	//Stop Train
	tp.tc = T_MOVE;
	tp.arg1 = tcargs.train;
	tp.arg2 = MEASURING_GEAR;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Set the starting node
	start = event.node;

	//To ensure train fully stopped
	Delay(cs_tid, my_tid, 10*3*MEASURING_GEAR);

	//Move Train
	tp.tc = T_MOVE;
	tp.arg1 = tcargs.train;
	tp.arg2 = tcargs.gear;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Delay Acceleration time
	Delay(cs_tid, my_tid, tcargs.delay);

	//Move Train
	tp.tc = T_MOVE;
	tp.arg1 = tcargs.train;
	tp.arg2 = 0;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));	

	//Delay
	Delay(cs_tid, my_tid, 10*3*tcargs.gear);	

	//Move Train
	tp.tc = T_MOVE;
	tp.arg1 = tcargs.train;
	tp.arg2 = MEASURING_GEAR;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));	

	//Calculate start time
	stime = Time(cs_tid, my_tid);

	//Get Current Position
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(event.node->type == NODE_SENSOR &&
			event.num == tcargs.train){
			break;
		}	
	}

	//Calculate end time
	etime = Time(cs_tid, my_tid);

	//Save data
	end = event.node;

	//Move Train
	tp.tc = T_MOVE;
	tp.arg1 = tcargs.train;
	tp.arg2 = 0;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	dist = dist_between_nodes(switches, start, end)*1000 - (etime - stime)*measuring_velocity - tcargs.dist;	

	TMLogStrf(tm_tid, "Accel Dist: %d\n", dist);
	Exit();
}
