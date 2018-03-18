#include <src/user/train/virtual_provider.h>

CIRCULAR_BUFFER_DEC(vereq_cb, VERequest, VEREQ_BUFFER_SIZE);

void VirtualEventPublisher(){
	int r;
	tid_t req_tid, sub;
	VESubscribe ves;

	tid_cb subscribers;
	tid_cb_init(&subscribers);

	int r = RegisterAs(VIRTUAL_PUBLISHER_ID);
	assert(r == 0);

	while(true){
		Receive(&req_tid, &ves, sizeof(ves));

		switch(ves.type){
		  case VER_NOTIFY:
		  	Reply(req_tid, &r, sizeof(r));
		  	NOTIFY(&subscribers, &sub, ves.req, sizeof(ves.req));
		  	break;
		  case VER_SUBSCRIBE:
		  	tid_cb_push(&subscribers, req_tid);
		  	break;
		  default:
		    assert(0 && "Bad Command");
		}
	}

  Exit();
}

void VirtualProviderCourier(){
	VERequest ver, data;
	VESubscribe ves;
	int r;

	tid_t pub_id = WhoIs(VIRTUAL_PUBLISHER_ID);
	tid_t vp_tid = MyParentTid();
	assert(pub_tid > 0 && vp_tid > 0);

	while(true){
		ver.type = VER_NOTIFY_READY;
		Send(vp_tid, &ver, sizeof(ver), &data, sizeof(data));

		ves.type = VER_NOTIFY;
		ves.req = data;
		Send(pub_tid, &ves, sizeof(ves), &r, sizeof(r));
	}

	Exit();
}

void RegisterTimeout(void *args){
	int r;
	VERequest ver = *(VERequest *)args;
	tid_t my_tid = MyTid();
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	tid_t vp_tid = MyParentTid();
	assert(my_tid > 0 && cs_tid > 0 && vp_tid > 0);

	ver.type = VER_EVENT;

	DelayUntil(cs_tid, my_tid, ver.ve.timestamp);
	Send(vp_tid, &ver, sizeof(ver), &r, sizeof(r));
	Exit();
}

void VirtualProvider(){
	int key = 0;

	tid_t req_tid;
	VERequest ver;
	int r;
	bool courierFlag = false;

	vereq_cb requests;
	vereq_cb_init(&requests);

	r = RegisterAs(VIRTUAL_PROVIDER_ID);
	assert(r == 0);

	Create(29, &VirtualEventPublisher);
	tid_t courier = Create(29, &VirtualProviderCourier);

	while(true){

		if(requests.size > 0 && courierFlag){
			vereq_cb_pop(&requests, &ver);	//Re-using ver as temp
			Reply(courier, &ver, sizeof(ver));
			courierFlag = false;
		}

		Receive(&req_tid, &ver, sizeof(ver));

		switch(ver.type){
			case VER_REGISTER:
				ver.ve.key = key;
				key = (key + 1) % KEY_SIZE;
				CreateArgs(19, &RegisterTimeout, (void *)&ver, sizeof(ver)); //TODO: FIX PRIORITY
				ver.ve.type = VE_REG;
				vereq_cb_push(&requests, ver);
				Reply(req_tid, &r, sizeof(r));
				break;
			case VER_EVENT:
				vereq_cb_push(&requests, ver);
				Reply(req_tid, &r, sizeof(r));
				break;
			case VER_NOTIFY_READY:
				courierFlag = true;
				break;
			default:
				assert(0 && "Bad Command");
				break;
		}
	}

	Exit();
}