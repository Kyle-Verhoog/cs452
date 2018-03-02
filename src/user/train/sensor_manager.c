#include <sensor_manager.h>
#include <lib/circular_buffer.h>


void UpdateSensorData(Sensor *slist, char byte, int scounter){
	int i=7;
	for(i = 7; i >= 0; i--){
		slist[i + scounter*8].state = byte & 1;
		byte = byte >> 1;
		assert(i + scounter*8 < 80);
		assert(i + scounter*8 >= 0);
	}
}

void PrintSensorData(tid_t ws_tid, Sensor *slist, rec_buffer *rb){
	int i;
	char sensor[4];
	int size;

	for(i = 0; i < SENSOR_SIZE; i++){
		if(slist[i].state){
			rec_buffer_add(rb, i);
		}
	}

	Cursor c;	//TODO: MAKE DEFINES
	SET_CURSOR(c, 3, 25);

	//Print from Latest Sensor
	for(i = 0; i < rb->num; i++){
		int val = rec_buffer_get(rb, i);
		sensor[0] = val/16 + 'A';
		i2a(val%16 + 1, &size, sensor+1);
		if(val%16 + 1 < 10){
			sensor[2] = ' ';
		}
		WriteCommandUART2(ws_tid, sensor, 3, &c);
		c.row++;
	}
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

void SensorManager(void *args){
	track_node *track = (track_node *)args;
	Sensor sensorList[SENSOR_SIZE];
	init_sensors(sensorList, track);

	rec_buffer rb;
	rec_buffer_init(&rb);

	int reply = 0;
	int r = RegisterAs(SENSOR_MANAGER_ID);
  	assert(r == 0);
  	tid_t ws_tid2 = WhoIs(WRITERSERVICE_UART2_ID);
    assert(ws_tid2 >= 0);
    tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
    assert(tx_tid >= 0);

    // Cursor c;
    // SET_CURSOR(c, 30, 20);

    Create(30, &SensorReceiver);
  	Create(30, &SensorTimeout);
  	int scounter = 0;
  	int recFlag = 0;
  	//Kick start sensor gathering data
  	PutC(tx_tid, GET_ALL_SENSORS);
  	while(true){
  		tid_t tid_req;
  		SMProtocol smp;

  		Receive(&tid_req, &smp, sizeof(smp));

  		switch(smp.smr){
  			case SM_READBYTE:
  				Reply(tid_req, &reply, sizeof(reply));
  				UpdateSensorData(sensorList, smp.byte, scounter);
  				scounter = (scounter + 1) % (DECODER_SIZE*2);
  				if(scounter == 0){
  					PutC(tx_tid, GET_ALL_SENSORS);
  					PrintSensorData(ws_tid2, sensorList, &rb);
  				}
  				if(scounter % 2 == 0){
  					recFlag = 1;
  				}
  				break;
  			case SM_CHECK:
  				Reply(tid_req, &recFlag, sizeof(recFlag));
  				recFlag = 0;
  				break;
  			case SM_RESET:
  				recFlag = 0;
  				scounter = 0;
  				PutC(tx_tid, GET_ALL_SENSORS);
  				Reply(tid_req, &reply, sizeof(reply));

  				// SHIFT_CURSOR(c, 1, 0);
  				break;
  			case SM_HALT:
  				break;
  			default:
  				assert(0 && "Bad Sensor Command");
  		}
  	}

  	Exit();
}
