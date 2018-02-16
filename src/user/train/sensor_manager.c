#include <sensor_manager.h>
#include <lib/circular_buffer.h>

#define CENSOR_DATA_RETENTION 20

CIRCULAR_BUFFER_DEC(sensor_cb, int, CENSOR_DATA_RETENTION);
CIRCULAR_BUFFER_DEF(sensor_cb, int, CENSOR_DATA_RETENTION);

void UpdateSensorData(Sensor *slist, char byte, int scounter){
	int i;
	for(i = 7; i >= 0; --i){
		slist[i + scounter*8] = byte & 1;
		byte = byte >> 1;	
	}
}

void PrintSensorData(Sensor *slist, sensor_cb *scb, int scounter){
	int i;
	int base = scounter*8;
	char sensor[3];
	int size;

	for(i = base; i < base + 8; i++){
		if(slist[i]){
			if(scb->size >= CENSOR_DATA_RETENTION){
				sensor_cb_pop(scb, NULL);
			}
			sensor_cb_push(scb, i);
		}
	}

	Cursor c;	//TODO: MAKE DEFINES
	c.row = 3;
	c.col = 20;
	//Print from Latest Sensor
	int ptr = i = (scb->end - 1 + CENSOR_DATA_RETENTION)%CENSOR_DATA_RETENTION;
	for(i = 0; i < CENSOR_DATA_RETENTION; i++){
		i2a(scb->buf[ptr], &size, sensor);
		//WriteCommandUART2(tx2_tid, sensor, size, &c); //TODO: IMPL THIS	
		c.row++;
	}
}

void SensorTimeout(){
	int reply = 0;
	tid_t sensor_man = MyParentTid();
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	tid_t my_tid = MyTid();
	int counter = -5;

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
			assert(0 && "Sensor data timed out");
			Send(sensor_man, &timeout, sizeof(timeout), &reply, sizeof(reply));
			counter = 0;
		}

		Delay(cs_tid, my_tid, 1);
	}
}

void SensorReciever(void *args){
	int reply = 0;
	tid_t rx1_tid = (tid_t)args;
	tid_t sensor_man = MyParentTid();
	tid_t my_tid = MyTid();
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	SMProtocol smp;
	smp.smr = SM_READBYTE;

	while(true){
		//smp.byte = GetC(rx1_tid);
		Delay(cs_tid, my_tid, 2);
		Send(sensor_man, &smp, sizeof(smp), &reply, sizeof(reply));
	}

	Exit();
}

void SensorManager(){
	Sensor SensorList[SENSOR_SIZE];
	sensor_cb scb;
	sensor_cb_init(&scb);

	int reply = 0;
	int r = RegisterAs(SENSOR_MANAGER_ID);
  	assert(r == 0);
  	tid_t rx1_tid = WhoIs(IOSERVER_UART1_RX_ID);
  	//assert(rx1_tid >= 0);
  	tid_t tx1_tid = WhoIs(IOSERVER_UART1_TX_ID);
  	//assert(tx1_tid >= 0);
  	tid_t rx2_tid = WhoIs(IOSERVER_UART2_RX_ID);
  	//assert(rx2_tid >= 0);
  	tid_t tx2_tid = WhoIs(IOSERVER_UART2_TX_ID);
  	//assert(tx2_tid >= 0);

  	CreateArgs(19, &SensorReciever, (void *)&rx1_tid);
  	Create(19, &SensorTimeout);
  	int scounter = 0;
  	int recFlag = 0;
  	//Kick start sensor gathering data
  	//PutC(tx1_tid, GET_ALL_SENSORS); //TODO: IMPL THIS
  	while(true){
  		tid_t tid_req;
  		SMProtocol smp;

  		Receive(&tid_req, &smp, sizeof(smp));

  		switch(smp.smr){
  			case SM_READBYTE:
  				Reply(tid_req, &reply, sizeof(reply));
  				UpdateSensorData(SensorList, smp.byte, scounter);
  				PrintSensorData(SensorList, &scb, scounter);
  				scounter = (scounter + 1) % (DECODER_SIZE*2);
  				if(scounter == 0){
  					//PutC(tx1_tid, GET_ALL_SENSORS); //TODO: IMPL THIS
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
  				//PutC(tx1_tid, GET_ALL_SENSORS); //TODO: IMPL THIS
  				Reply(tid_req, &reply, sizeof(reply));
  				break;
  			default:
  				assert(0 && "Bad Sensor Command");
  		}
  	}

  	Exit();
}