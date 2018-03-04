#include <prediction_manager.h>

//CIRCULAR_BUFFER_DEF(pm_cb, volatile PredictionModel, MAX_STEPS_AHEAD)

tid_t tm_tid;

void Oracle(void *args){

}

track_node *GetNextSensor(Switch *sw, track_node *n){
	do{
		if(n->type == NODE_BRANCH){
			if(sw[n->num].state == SW_STRAIGHT){
				n = n->edge[DIR_STRAIGHT].dest;
			}
			else{
				n = n->edge[DIR_CURVED].dest;	
			}
		}
		else{
			n = n->edge[DIR_AHEAD].dest;
		}

	} while(n->type != NODE_SENSOR);

	return n;
}

void CheckPrediction(Switch *swList, Sensor *snList, LiveTrains *live){
	int i;
	for(i = 0; i < live->size; i++){
		TrainDescriptor * train = live->buf[i];
		track_node * next = GetNextSensor(swList, train->node);
		if(snList[train->node->num].state == SEN_OFF){
			//Train Left Sensor
      TMLogStrf(tm_tid, "TRAIN %d LEFT SENSOR %s\n", train->id, train->node->name);
		}
		if(snList[next->num].state == SEN_ON){
			//Train On New Sensor
			TMLogStrf(tm_tid, "TRAIN %d ON SENSOR %s\n", train->id, next->name);
			train->node = next;
		}
	}
}

void AddTrain(LiveTrains *live, TrainDescriptor *td){
	live->buf[live->size] = td;
	live->size++;
}

void PredictionManager(void *args){
	track_node *track = (track_node *)args;
	int r = RegisterAs(PREDICTION_MANAGER_ID);
	assert(r == 0);

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

	LiveTrains live;
	INIT_LIVE_TRAINS(live);

	Switch *swList = NULL;
	Sensor *snList = NULL;

	while(true){
		int reply = 0;
		tid_t tid_req;
		PMProtocol pmp;

		Receive(&tid_req, &pmp, sizeof(pmp));
		Reply(tid_req, &reply, sizeof(reply));

		switch(pmp.pmc){
			case PM_SENSOR:
				//Sensor data has been updated
				snList = (Sensor *)pmp.args;
				if(swList != NULL && snList != NULL){
					CheckPrediction(swList, snList, &live);	
				}
				break;
			case PM_SWITCH:
				//Re-evaluate Predictions
				swList = (Switch *)pmp.args;
				break;
			case PM_TRAIN:
				AddTrain(&live, (TrainDescriptor *)pmp.args);
        TMLogStrf(tm_tid, "train %d on track %s\n", live.buf[live.size-1]->id, live.buf[live.size-1]->node->name);
				break;
			default:
				assert(0 && "Bad Command");
				break;
		}
	}
}
