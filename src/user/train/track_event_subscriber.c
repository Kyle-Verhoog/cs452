#include <user/train/track_event_subscriber.h>

void TrainGearEventSubscriber(){
	int r;
	tid_t rep_tid, par_tid;
	TrackRequest tr_req;
	TrackEvent te;
	TETRGear event;

	par_tid = MyParentTid();
	assert(par_tid > 0);
	rep_tid = WhoIs(REPRESENTER_ID);
	assert(rep_tid > 0);

	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_MOVE;

	te.type = TE_TR_MOVE;

	while (true) {
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		te.event.tr_gear = event;
		Send(par_tid, &te, sizeof(te), &r, sizeof(r));
	}

	Exit();
}

void TrainPositionEventSubscriber(){
	int r;
	tid_t rep_tid, par_tid;
	TrackRequest tr_req;
	TrackEvent te;
	TETRPosition event;

	par_tid = MyParentTid();
	assert(par_tid > 0);
	rep_tid = WhoIs(REPRESENTER_ID);
	assert(rep_tid > 0);

	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_POSITION;

	te.type = TE_TR_POSITION;

	while (true) {
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		te.event.tr_pos = event;
		Send(par_tid, &te, sizeof(te), &r, sizeof(r));
	}

	Exit();
}

void TrainSpeedEventSubscriber(){
	int r;
	tid_t rep_tid, par_tid;
	TrackRequest tr_req;
	TrackEvent te;
	TETRSpeed event;

	par_tid = MyParentTid();
	assert(par_tid > 0);
	rep_tid = WhoIs(REPRESENTER_ID);
	assert(rep_tid > 0);

	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_SPEED;

	te.type = TE_TR_SPEED;

	while (true) {
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		te.event.tr_speed = event;
		Send(par_tid, &te, sizeof(te), &r, sizeof(r));
	}

	Exit();
}

void TrainStatusEventSubscriber(){
	int r;
	tid_t rep_tid, par_tid;
	TrackRequest tr_req;
	TrackEvent te;
	TETRStatus event;

	par_tid = MyParentTid();
	assert(par_tid > 0);
	rep_tid = WhoIs(REPRESENTER_ID);
	assert(rep_tid > 0);

	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_TR_STATUS;

	te.type = TE_TR_STATUS;

	while (true) {
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		te.event.tr_status = event;
		Send(par_tid, &te, sizeof(te), &r, sizeof(r));
	}

	Exit();	
}

void SwitchEventSubscriber(){
	int r;
	tid_t rep_tid, par_tid;
	TrackRequest tr_req;
	TrackEvent te;
	TESWChange event;

	par_tid = MyParentTid();
	assert(par_tid > 0);
	rep_tid = WhoIs(REPRESENTER_ID);
	assert(rep_tid > 0);

	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_SW_CHANGE;

	te.type = TE_SW_CHANGE;

	while (true) {
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		te.event.sw_change = event;
		Send(par_tid, &te, sizeof(te), &r, sizeof(r));
	}

	Exit();
}

void SensorEventSubscriber(){
	int r;
	tid_t rep_tid, par_tid;
	TrackRequest tr_req;
	TrackEvent te;
	TESEChange event;

	par_tid = MyParentTid();
	assert(par_tid > 0);
	rep_tid = WhoIs(REPRESENTER_ID);
	assert(rep_tid > 0);

	tr_req.type = TRR_SUBSCRIBE;
	tr_req.data.type = TE_SE_CHANGE;

	te.type = TE_SE_CHANGE;

	while (true) {
		Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
		te.event.se_change = event;
		Send(par_tid, &te, sizeof(te), &r, sizeof(r));
	}

	Exit();
}

