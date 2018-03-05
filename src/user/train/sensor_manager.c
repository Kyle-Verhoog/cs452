#include <sensor_manager.h>
#include <lib/circular_buffer.h>
#include <prediction_manager.h>


void PrintSensorData(tid_t si_tid, Sensor *slist){
  int reply;
  Send(si_tid, &slist, sizeof(slist), &reply, sizeof(reply));
}

void init_sensors(Sensor *list, track_node *t){
	int i;
	for(i = 0; i < TRACK_MAX; i++){
		if(t[i].type == NODE_SENSOR){
			list->state = SEN_OFF;
			list->node = &t[i];
			++list;
		}
	}
}

void PushSensorToPrediction(tid_t pred, Sensor *list){
	int reply;
	PMProtocol pmp;
	pmp.pmc = PM_SENSOR;
	pmp.args = (void *)list;
	pmp.size = 1;

	Send(pred, &pmp, sizeof(pmp), &reply, sizeof(reply));
}

void Notify(tid_cb *subscribers){
	int reply = 1;
	tid_t sub;
	while(tid_cb_pop(subscribers, &sub) != CB_E_EMPTY){
		Reply(sub, &reply, sizeof(reply));
	}
}

//Returns if a diff happened
bool UpdateSensorData(Sensor *slist, char byte, int scounter, tid_cb* subscribers){
	bool delta = false;
	int i=7;
	for(i = 7; i >= 0; i--){
		delta = delta || (slist[i + scounter*8].state != (byte & 1));
		slist[i + scounter*8].state = byte & 1;
		byte = byte >> 1;
		assert(i + scounter*8 < 80);
		assert(i + scounter*8 >= 0);

		if(slist[i + scounter*8].state == SEN_ON){
			Notify(&subscribers[i + scounter*8]);
		}
	}

	return delta;
}

void SensorTimeout(){
	int reply = 0;
	tid_t sensor_man = MyParentTid();
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	tid_t my_tid = MyTid();
	volatile int counter = 0;

	SMProtocol checker, timeout;
	checker.smr = SM_CHECK;
	timeout.smr = SM_RESET;

	while(true){
		//Check if Sensor data is being retrieved
		Send(sensor_man, &checker, sizeof(checker), &reply, sizeof(reply));
		if(reply){
			counter = 0;
		}
		else{
			counter++;
		}

		//Too much time has passed since data was retrieved
		if(counter >= SENSOR_TIMEOUT){
			Send(sensor_man, &timeout, sizeof(timeout), &reply, sizeof(reply));
			counter = 0;
		}

		Delay(cs_tid, my_tid, SENSOR_WAIT);
	}
}

void SensorReceiver(){
	int reply = 0;
	tid_t sensor_man = MyParentTid();
	tid_t rx_tid = WhoIs(IOSERVER_UART1_RX_ID);
	SMProtocol smp;
	smp.smr = SM_READBYTE;

	while(true){
		smp.byte = GetC(rx_tid);
		// Delay(cs_tid, my_tid, 2);
		Send(sensor_man, &smp, sizeof(smp), &reply, sizeof(reply));
	}

	Exit();
}

void init_subscribers(tid_cb *subscribers){
	int i;
	for(i = 0; i < SENSOR_SIZE + 1; i++){
		tid_cb_init(&subscribers[i]);
  }
}

void SensorManager(void *args){
	void *data;
	int scounter = 0;
  	bool recFlag = false;
  	bool deltaFlag = false;

	track_node *track = (track_node *)args;
	Sensor sensorList[SENSOR_SIZE];
	init_sensors(sensorList, track);

	tid_cb subscribers[SENSOR_SIZE + 1];	//Subscribers on [SENSOR_SIZE] are waiting on delta
	init_subscribers(subscribers);

	int reply = 0;
	int r = RegisterAs(SENSOR_MANAGER_ID);
  	assert(r == 0);
  	tid_t pred_tid = WhoIs(PREDICTION_MANAGER_ID);
    assert(pred_tid >= 0);
    tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
    assert(tx_tid >= 0);


    // Cursor c;
    // SET_CURSOR(c, 30, 20);

    Create(30, &SensorReceiver);
  	Create(30, &SensorTimeout);

    tid_t si_tid = Create(29, &SensorInterface);
  	//Kick start sensor gathering data
  	BLPutC(tx_tid, GET_ALL_SENSORS);
  	while(true){
  		tid_t tid_req;
  		SMProtocol smp;

  		Receive(&tid_req, &smp, sizeof(smp));

  		switch(smp.smr){
  			case SM_READBYTE:
  				Reply(tid_req, &reply, sizeof(reply));
  				deltaFlag = deltaFlag || UpdateSensorData(sensorList, smp.byte, scounter, subscribers);
  				scounter = (scounter + 1) % (DECODER_SIZE*2);
  				if(scounter == 0){
  					BLPutC(tx_tid, GET_ALL_SENSORS);
  					//Update Prediction
  					if(deltaFlag){
  						Notify(&subscribers[SENSOR_SIZE]);
  						PushSensorToPrediction(pred_tid, sensorList);
  						deltaFlag = false;
              PrintSensorData(si_tid, sensorList);
  					}
  				}
  				if(scounter % 2 == 0){
  					recFlag = true;
  				}
  				break;
  			case SM_CHECK:
  				Reply(tid_req, &recFlag, sizeof(recFlag));
  				recFlag = false;
  				break;
  			case SM_RESET:
  				Reply(tid_req, &reply, sizeof(reply));
  				recFlag = false;
  				scounter = 0;
  				BLPutC(tx_tid, GET_ALL_SENSORS);
  				break;
  			case SM_SUBSCRIBE:
  				tid_cb_push(&subscribers[(int)smp.byte], tid_req);
  				//Reply(tid_req, &reply, sizeof(reply));
  				break;
  			case SM_GET_ALL:
  				data = (void *)sensorList;
  				Reply(tid_req, &data, sizeof(data));
  				break;
  			case SM_HALT:
  				Reply(tid_req, &reply, sizeof(reply));
  				break;
  			default:
  				assert(0 && "Bad Sensor Command");
  		}
  	}

  	Exit();
}
