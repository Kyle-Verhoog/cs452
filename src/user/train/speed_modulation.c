#include <user/train/speed_modulation.h>

static void SendSpeed(tid_t tr_tid, int train, int gear){
	int r;
	TrainProtocol tp;

	//Send Speed
	tp.tc = T_MOVE;
	tp.arg1 = train;
	tp.arg2 = gear;
	Send(tr_tid, &tp, sizeof(tp), &r, sizeof(r));
}

void SpeedModulator(void *args){
	int r, lower, upper, target;
	tid_t cs_tid, tm_tid, parent_tid, tr_tid;
	TrainModel s_model, stp_model, acl_model;
	Modulator mod;

	mod = *(Modulator *)args;
	mod.mr = MR_REPORT_RES;

	parent_tid = MyParentTid();
	cs_tid = WhoIs(CLOCKSERVER_TID);
	tr_tid = WhoIs(TRAIN_PROVIDER_TID);
	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	assert(parent_tid > 0 && cs_tid > 0 && tr_tid > 0 && tm_tid > 0);

	//Get Models
	getVelocityModel(s_model, mod.train_num);

	target = estimateGear(s_model.x, s_model.y, mod.velocity);
	lower = (target / 10) * 10;
	upper = lower + 10;

	//TODO: Need current gear (and move to lower bound gear)
	while(true){
		Send(parent_tid, &mod, sizeof(mod), &r, sizeof(r));
		switch(r){
			case CONTINUE:

			case SUSPEND:
				//Delay
				break;
			case EXIT:
				Exit();
				break; 
		}
	}

	Exit();
}

void SpeedModulationService(){
	int r, state;
	tid_t tm_tid, sm_tid, req_tid;
	Modulator mod;

	tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	state = CONTINUE;

	while(true){
		Receive(&req_tid, &mod, sizeof(mod));

		switch(mod.mr){
			case MR_MOD_REQ:
				sm_tid = CreateArgs(19, &SpeedModulator, (void *)mod, sizeof(mod));
				Reply(req_tid, r, sizeof(r));
				break;

			case MR_SUSPEND_REQ:
				state = SUSPEND;
				Reply(req_tid, state, sizeof(state));
				break;

			case MR_QUIT_REQ:
				state = EXIT;
				Reply(req_tid, state, sizeof(state));
				break;				

			case MR_REPORT_RES:
				Reply(req_tid, state, sizeof(state));
				Exit();
				break;
			default:
				assert(0 && "Bad Command");
		}
	}

	Exit();
}