#include <sensor_provider.h>
#include <lib/circular_buffer.h>
#include <prediction_manager.h>

//Returns if a diff happened
// bool UpdateSensorData(Sensor *slist, char byte, int scounter){
// 	bool delta = false;
// 	int i=7;
// 	for(i = 7; i >= 0; i--){
// 		delta = delta || (slist[i + scounter*8].state != (byte & 1));
// 		slist[i + scounter*8].state = byte & 1;
// 		byte = byte >> 1;
// 		assert(i + scounter*8 < 80);
// 		assert(i + scounter*8 >= 0);
// 	}

// 	return delta;
// }

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
		Send(sensor_man, &smp, sizeof(smp), &reply, sizeof(reply));
	}

	Exit();
}

void SensorPublisher(){
	int reply;
	tid_t tid_req, sub;
  	SMSubscribe sms;

	tid_cb subscribers;
	tid_cb_init(&subscribers);

	int r = RegisterAs(SENSOR_PUBLISHER_ID);
	assert(r == 0);

	while(true){
		Receive(&tid_req, &sms, sizeof(sms));

		switch(sms.smr){
			case SM_NOTIFY:
				Reply(tid_req, &reply, sizeof(reply));
				NOTIFY(&subscribers, &sub, sms.sensors, sizeof(sms.sensors));
				break;
			case SM_SUBSCRIBE:
				tid_cb_push(&subscribers, tid_req);
				break;
			default:
  				assert(0 && "Bad Command");
		}
	}

	Exit();
}

void SensorUpdateCourier(){
	SMProtocol smp;
	SMSubscribe sms;
	int reply;

	tid_t pub_tid = WhoIs(SENSOR_PUBLISHER_ID);
	tid_t sm_tid = MyParentTid();

	while(true){
		smp.smr = SM_NOTIFY_READY;
		Send(sm_tid, &smp, sizeof(smp), &sms.sensors, sizeof(sms.sensors));

		sms.smr = SM_NOTIFY;
		Send(pub_tid, &sms, sizeof(sms), &reply, sizeof(reply));	
	}
	
	Exit();
}

void SensorProvider(){
	int scounter = 0;
	int reply;
  	bool recFlag = false;
  	bool deltaFlag = false;
  	bool persistDeltaFlag = false;
  	bool courierFlag = false;
  	bool dataFlag = false;

  	char sensors[DECODER_SIZE*2];

	int reply = 0;
    tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
    assert(tx_tid >= 0);

    Create(29, &SensorReceiver);
  	Create(29, &SensorTimeout);
  	//CreateArgs(29, &SensorPublisher, &sl, sizeof(Sensor *));
  	Create(29, &SensorPublisher);
    tid_t suc_tid = Create(29, &SensorUpdateCourier);

  	//Kick start sensor gathering data
  	BLPutC(tx_tid, GET_ALL_SENSORS);
  	while(true){
  		tid_t tid_req;
  		SMProtocol smp;

  		//If courier is ready, send the data
  		if(dataFlag && courierFlag){
  			dataFlag = false;
  			courierFlag = false;
  			Reply(suc_tid, &sensors, sizeof(sensors));
  		}

  		Receive(&tid_req, &smp, sizeof(smp));

  		switch(smp.smr){
  			case SM_READBYTE:
  				Reply(tid_req, &reply, sizeof(reply));
  				sensors[scounter] = smp.byte;
  				scounter = (scounter + 1) % (DECODER_SIZE*2);
  				if(scounter == 0){
  					BLPutC(tx_tid, GET_ALL_SENSORS);
  					dataFlag = true;
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
  			case SM_NOTIFY_READY:
  				courierFlag = true;
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
