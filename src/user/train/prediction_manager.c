#include <prediction_manager.h>
#include <stoppingcalibration_test.h>

tid_t tm_tid;

void Oracle(void *args){
}

void MeasureSpeed(tid_t mytid, tid_t cs_tid, TrainDescriptor *td, int dist){
  int prev_speed = td->speed;
  int pred_time = (dist / prev_speed); // predicted time of our model
	int time = Time(cs_tid, mytid);
	int speed = (dist * 1000)/(time - td->time_of_sensor);
  int delta_t = time - pred_time;

	if(time <= td->time_of_sensor){
		assert(0);
	}

	td->speed = speed;
	td->time_of_sensor = time;
  TMLogStrf(tm_tid, "t: %d; d: %d\n", delta_t, prev_speed*delta_t);
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

Destination GetPrevSensorNI(Switch *sw, track_node *n){
	Destination dest;
	dest.dist = 0;
	n = n->reverse;

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
	}while(n->type != NODE_SENSOR);

	dest.node = n->reverse;

	return dest;
}

Destination GetNextSensor(Switch *sw, track_node *n){
	Destination dest;
	dest.dist = 0;

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

	dest.node = n;

	return dest;
}

Destination GetNextSensorNI(Switch *sw, track_node *n){
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
		dest = GetNextSensorNI(swList, train->node);
		if(train->dir != dir){
			assert(0 && "TRAIN HAS FLIPPED SOMEWHERE WHEN WE CALLED NEXT SENSOR");
		}
		if(dest.node->type == NODE_EXIT){
			assert(0 && "TRAIN IS ABOUT TO RUN OFF THE TRACK");
		}
		else{
      //Train Left Sensor
      if(snList[train->node->num].state == SEN_OFF){
        TMLogStrf(tm_tid, "left %s\n", train->node->name);
      }
      //Train On New Sensor
      if(snList[dest.node->num].state == SEN_ON){
        train->node = dest.node;
        MeasureSpeed(mytid, cs_tid, train, dest.dist);
        // if (dest.node == train->tpath.start) {
        //   path_start(&train->tpath, train->tpath.start);
        //   TMLogStrf(tm_tid, "starting pathing");
        // }
      }
		}
	}
}

void MeasuringInitTask(void * args){
	Switch *switches;
	InchingArgs iargs;

	tid_t my_tid = MyTid();
	tid_t term_tid = WhoIs(TERMINAL_MANAGER_ID);
  	assert(term_tid >= 0);
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	assert(cs_tid >= 0);
	tid_t tm_tid = WhoIs(TRAIN_MANAGER_ID);
	assert(tm_tid >= 0);
	tid_t sw_tid = WhoIs(SWITCH_MANAGER_ID);
	assert(sw_tid >= 0);
	tid_t sm_tid = WhoIs(SENSOR_MANAGER_ID);
	assert(sm_tid >= 0);

	SWProtocol sw;
	
	//Get Switch List
	sw.swr = SW_GET_ALL;
	Send(sw_tid, &sw, sizeof(sw), &switches, sizeof(switches));
	
	iargs.tm_tid = tm_tid;
	iargs.sm_tid = sm_tid;
	iargs.cs_tid = cs_tid;
	iargs.my_tid = my_tid;
	iargs.switches = switches;
	iargs.train = (TrainDescriptor *)args;

	//Call Initialize
	TMLogStrf(term_tid, "measuring train %d inching\n", iargs.train->id);
	InitInch(iargs);

	Exit();
}

void AddTrain(LiveTrains *live, TrainDescriptor *td){
	live->buf[live->size] = td;
	live->size++;
	TMLogStrf(tm_tid, "train %d on track %s\n", td->id, td->node->name);
	//MeasureTrain(td);
}

void MeasureTrain(TrainDescriptor *td){
	CreateArgs(21, &MeasuringInitTask, (void*)td);
}

#define SW_LOOK_AHEAD 500000
/*
void Routing(tid_t sw_tid, Switch *sws, LiveTrains *live) {
  assert(live->size > 0);
  int i, reply;
  sw_configs sw_cfgs;
  sw_config sw_cfg;
  track_node *tr_tn, *sw_tn;
  TrainDescriptor *td;
  td = live->buf[0];
  tr_tn = td->node;

  if (td->tpath.active) {
    if (path_follow_to(&td->tpath, tr_tn) < 0) {
      TMLogStrf(tm_tid, "node not found in path! %s\n", tr_tn->name);
    }
    TMLogStrf(tm_tid, "path pos %s\n", td->tpath.current->name);
    path_switches_in_next_dist(&td->tpath, &sw_cfgs, SW_LOOK_AHEAD);

    for (i = 0; i < sw_cfgs.size; ++i) {
      TMLogStrf(tm_tid, "checking sws\n");
      sw_configs_pop(&sw_cfgs, &sw_cfg);
      sw_tn = sw_cfg.sw;

      SWProtocol sw;
      if (sw_cfg.state_required != sws[sw_tn->num].state - 33) {
        TMLogStrf(tm_tid, "  sw %d %d\n", sw_tn->num, sw_cfg.state_required);
        sw.swr = SW_FLIP;
        sw.sw = sw_tn->num;
        sw.dir = sw_cfg.state_required + 33;
        Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
      }
    }

    if (path_arrived(&td->tpath)) {
      TMLogStrf(tm_tid, "arrived!\n");
    }
  }
}
*/

void SetRoute(LiveTrains *live, track_node *s, track_node *e, tid_t sw_tid, Switch *sws) {
  assert(live->size > 0);
  int i, reply;
  char buf[1024];
  TrainDescriptor *td;
  path *p;
  td = live->buf[0];
  p = &td->tpath;
  path_init(p, p->track);
  path_set_destination(p, s, e);
  if (path_generate(p) > -1) {
    path_to_str(p, buf);
    TMLogStrf(tm_tid, "route calculated:\n", buf);
    TMLogStrf(tm_tid, "%s\n", buf);


    track_node *sw_tn;
    sw_configs sw_cfgs;
    sw_configs_init(&sw_cfgs);
    sw_config sw_cfg;
    path_switches_in_next_dist(&td->tpath, &sw_cfgs, SW_LOOK_AHEAD);
    TMLogStrf(tm_tid, "checking sws\n");
    for (i = 0; i < sw_cfgs.size; ++i) {
      sw_configs_get(&sw_cfgs, sw_cfgs.size-i-1, &sw_cfg);
      sw_tn = sw_cfg.sw;

      SWProtocol sw;
      if (sw_cfg.state_required != sws[sw_tn->num].state - 33) {
        TMLogStrf(tm_tid, "  sw %d %d\n", sw_tn->num, sw_cfg.state_required);
        sw.swr = SW_FLIP;
        sw.sw = sw_tn->num;
        sw.dir = sw_cfg.state_required + 33;
        Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
      }
    }
  }
  else {
    TMLogStrf(tm_tid, "no route found\n", buf);
  }
}


void PredictionManager(void *args){
	track_node *track = (track_node *)args;
  // int path_prev[]
	int r = RegisterAs(PREDICTION_MANAGER_ID);
	assert(r == 0);

	tid_t mytid = MyTid();
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	assert(cs_tid >= 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	assert(tm_tid >= 0);
	tid_t tr_tid = WhoIs(TRAIN_MANAGER_ID);
  assert(tr_tid >= 0);
	tid_t sw_tid = WhoIs(SWITCH_MANAGER_ID);
  assert(sw_tid >= 0);

	LiveTrains live;
	INIT_LIVE_TRAINS(live);

	Switch *swList = NULL;
	Sensor *snList = NULL;

  int *route_args;

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
          // Routing(sw_tid, swList, &live);
					CheckPrediction(mytid, cs_tid, swList, snList, &live);	
				}
				break;
			case PM_SWITCH:
				//Re-evaluate Predictions
				swList = (Switch *)pmp.args;
				break;
      // case PM_STOP:
        // break;
      case PM_ROUTE:
        route_args = (int *)pmp.args;
        SetRoute(&live, &track[route_args[0]], &track[route_args[1]], sw_tid, swList);
        break;
			case PM_TRAIN:
				AddTrain(&live, (TrainDescriptor *)pmp.args);
				break;
			case PM_MEASURE:
				//Init the train
				MeasureTrain((TrainDescriptor *)pmp.args);
				break;
			default:
				assert(0 && "Bad Command");
				break;
		}
	}
}
