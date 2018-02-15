#include <train_manager.h>

void TrainManager(){
	int reply = 0;
	int r = RegisterAs(TRAIN_MANAGER_ID);
  	assert(r == 0);

	while(true){
		tid_t tid_req;
		TMProtocol tmp;

		Receive(&tid_req, &tmp, sizeof(tmp));
		Reply(tid_req, &reply, sizeof(reply));

		switch(tmp.trc){
			case TR_TRAIN:
				assert(0 && "TR_TRAIN");
				break;
			case TR_REVERSE:
				assert(0 && "TR_REVERSE");
				break;
			case TR_SWITCH:
				assert(0 && "TR_SWITCH");
				break;
			default:
				assert(0 && "Bad Train Command");
				break;
		}
	}
}
