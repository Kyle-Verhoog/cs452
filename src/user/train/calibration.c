#include <user/train/calibration.h>

int measuring_velocity;

void Calibration(void *args){
	int r, stime, etime;
	CalibrationArgs cargs;
	TrackRequest tr_req;
	tid_t cs_tid, sub_tid, tm_tid, rep_tid, tr_tid;
	TETRPosition event;
	TrainProtocol tp;
	track_node *start, *end;
	Switch switches[SWITCH_SIZE];


	cargs = *(Calibration *)args;

	my_tid = MyTid();
	cs_tid = WhoIs(CLOCKSERVER_ID);
	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	rep_tid = WhoIs(REPRESENTER_ID);
	tr_tid = WhoIs(TRAIN_PROVIDER_ID);
	assert(tm_tid > 0 && cs_tid > 0 && rep_tid > 0, tr_tid > 0);

	//Initialize subscription
	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_POSITION;

	//Send Speed
	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = cargs.init_speed;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = 0;

	//Wait until starting point
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(TRACK[event.node]->num == cargs.target_node &&
			event.num == cargs.train){
			break;
		}	
	}

	//Send stop
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));

	//Wait for train to stop
	Delay(cs_tid, my_tid, 10*3*cargs.init_speed);	//Some arbitrary delay to ensure

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
		if(TRACK[event.node]->type == NODE_SENSOR &&
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
	end = &TRACK[event.node];

	
	
	Exit();
}

void MeasuringVelocity(){
	int r, stime, etime;
	TrackRequest tr_req;
	tid_t cs_tid, sub_tid, tm_tid, rep_tid, tr_tid;
	TETRPosition event;
	track_node *start, *end;
	TrainProtocol tp;

	my_tid = MyTid();
	cs_tid = WhoIs(CLOCKSERVER_ID);
	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	rep_tid = WhoIs(REPRESENTER_ID);
	tr_tid = WhoIs(TRAIN_PROVIDER_ID);
	assert(tm_tid > 0 && cs_tid > 0 && rep_tid > 0, tr_tid > 0);

	//Initialize subscription
	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_POSITION;

	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = MEASURING_GEAR;

	//Wait until starting point
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(TRACK[event.node]->type == NODE_SENSOR &&
			event.num == cargs.train){
			break;
		}	
	}

	stime = Time(cs_tid, my_tid);
	start = &TRACK[event.node];

	//Wait until end point
	while(true){
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		if(TRACK[event.node]->type == NODE_SENSOR &&
			event.num == cargs.train){
			break;
		}	
	}

	etime = Time(cs_tid, my_tid);
	end = &TRACK[event.node];

	//Send Stop
	tp.tc = T_MOVE;
	tp.arg1 = cargs.train;
	tp.arg2 = 0;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));	

	//Get Switch Configuration
	tr_req.type = TRR_FETCH;
	tr_req.data.dtype = TD_SW;
	Send(rep_tid, &tr_req, sizeof(tr_req), &switches, sizeof(switches));

	//COMPUTE THE VELOCITY


	Exit();
}