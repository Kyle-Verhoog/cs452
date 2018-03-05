#include <prediction_manager.h>

//CIRCULAR_BUFFER_DEF(pm_cb, volatile PredictionModel, MAX_STEPS_AHEAD)


tid_t tm_tid;

void Oracle(void *args){
}

void MeasureSpeed(tid_t mytid, tid_t cs_tid, TrainDescriptor *td, int dist){
	int time = Time(cs_tid, mytid);
	int speed = dist * 1000/(time - td->time_of_sensor);

	if(time <= td->time_of_sensor){
		assert(0);
	}

	td->speed = speed;
	td->time_of_sensor = time;
}

int DistanceBetweenNodes(Switch *sw, track_node *start, track_node *end){
	int dist = 0;
	track_node *n = start; 
	while(n != end){
		if(n->type == NODE_EXIT){
			return -1;
		}
		else if(n->type == NODE_BRANCH){
			if(sw[n->num].state == SW_STRAIGHT){
				dist += n->edge[DIR_STRAIGHT].dist;
				n = n->edge[DIR_STRAIGHT].dest;
			}
			else{
				dist += n->edge[DIR_CURVED].dist;
				n = n->edge[DIR_CURVED].dest;
			}
		}
		else{
			dist += n->edge[DIR_AHEAD].dist;
			n = n->edge[DIR_AHEAD].dest;
		}
	}

	return dist;
}

Destination GetPrevSensor(Switch *sw, track_node *n){
	Destination dest;
	dest.dist = 0;
	n = n->reverse;

	while(n->type != NODE_SENSOR){
		if(n->type == NODE_EXIT){
			break;
		}
		else if(n->type == NODE_BRANCH){
			if(sw[n->num].state == SW_STRAIGHT){
				dest.dist += n->edge[DIR_STRAIGHT].dist;
				n = n->edge[DIR_STRAIGHT].dest;
			}
			else{
				dest.dist += n->edge[DIR_CURVED].dist;
				n = n->edge[DIR_CURVED].dest;
			}
		}
		else{
			dest.dist += n->edge[DIR_AHEAD].dist;
			n = n->edge[DIR_AHEAD].dest;
		}
	}

	dest.node = n->reverse;

	return dest;
}

Destination GetNextSensor(Switch *sw, track_node *n){
	Destination dest;
	dest.dist = 0;

	do{
		if(n->type == NODE_EXIT){
			break;
		}
		else if(n->type == NODE_BRANCH){
			if(sw[n->num].state == SW_STRAIGHT){
				dest.dist += n->edge[DIR_STRAIGHT].dist;
				n = n->edge[DIR_STRAIGHT].dest;
			}
			else{
				dest.dist += n->edge[DIR_CURVED].dist;
				n = n->edge[DIR_CURVED].dest;
			}
		}
		else{
			dest.dist += n->edge[DIR_AHEAD].dist;
			n = n->edge[DIR_AHEAD].dest;
		}

	} while(n->type != NODE_SENSOR);

	dest.node = n;

	return dest;
}

void CheckPrediction(tid_t mytid, tid_t cs_tid, Switch *swList, Sensor *snList, LiveTrains *live){
	Destination dest;
	int i;
	bool dir;

	for(i = 0; i < live->size; i++){
		TrainDescriptor * train = live->buf[i];
		dir = train->dir;
		dest = GetNextSensor(swList, train->node);
		if(train->dir != dir){
			assert(0 && "TRAIN HAS FLIPPED SOMEWHERE WHEN WE CALLED NEXT SENSOR");
		}
		if(dest.node->type == NODE_EXIT){
			assert(0 && "TRAIN IS ABOUT TO RUN OFF THE TRACK");
		}
		else{
			if(snList[train->node->num].state == SEN_OFF){
			//Train Left Sensor
      TMLogStrf(tm_tid, "tr %d left %s\n", train->id, train->node->name);
			//PRINTF("TRAIN %d LEFT SENSOR %s\n\r", train->id, train->node->name);
		}
			if(snList[dest.node->num].state == SEN_ON){
				//Train On New Sensor
				train->node = dest.node;
				MeasureSpeed(mytid, cs_tid, train, dest.dist);
			  TMLogStrf(tm_tid, "tr %d at %s (%d)\n", train->id, dest.node->name, train->speed);
				//PRINTF("TRAIN %d ON SENSOR %s with SPEED: %dum/ms\n\r", train->id, dest.node->name, train->speed);
			}
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

	tid_t mytid = MyTid();
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	assert(cs_tid >= 0);
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
					CheckPrediction(mytid, cs_tid, swList, snList, &live);	
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
