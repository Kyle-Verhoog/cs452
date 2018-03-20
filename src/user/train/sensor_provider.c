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

	SNProtocol checker, timeout;
	checker.snr = SN_CHECK;
	timeout.snr = SN_RESET;

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
	SNProtocol snp;
	snp.snr = SN_READBYTE;

	while(true){
		snp.byte = GetC(rx_tid);
		Send(sensor_man, &snp, sizeof(snp), &reply, sizeof(reply));
	}

	Exit();
}

void SensorPublisher(){
	int reply;
	tid_t tid_req, sub;
  	SNSubscribe sns;

	tid_cb subscribers;
	tid_cb_init(&subscribers);

	int r = RegisterAs(SENSOR_PUBLISHER_ID);
	assert(r == 0);

	while(true){
		Receive(&tid_req, &sns, sizeof(sns));

		switch(sns.snr){
			case SN_NOTIFY:
				Reply(tid_req, &reply, sizeof(reply));
				NOTIFY(&subscribers, &sub, sns.sensors, sizeof(sns.sensors));
				break;
			case SN_SUBSCRIBE:
				tid_cb_push(&subscribers, tid_req);
				break;
			default:
  				assert(0 && "Bad Command");
		}
	}

	Exit();
}

void SensorUpdateCourier(){
	SNProtocol snp;
	SNSubscribe sns;
	int reply;

	tid_t pub_tid = WhoIs(SENSOR_PUBLISHER_ID);
	tid_t sn_tid = MyParentTid();

	while(true){
		snp.snr = SN_NOTIFY_READY;
		Send(sn_tid, &snp, sizeof(snp), &sns.sensors, sizeof(sns.sensors));

		sns.snr = SN_NOTIFY;
		Send(pub_tid, &sns, sizeof(sns), &reply, sizeof(reply));	
	}
	
	Exit();
}

void TestSNPublisher(){
	SNSubscribe sns;
	char sensors[DECODER_SIZE*2];

	tid_t pub_tid = WhoIs(SENSOR_PUBLISHER_ID);
	assert(pub_tid >= 0);
	tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
	assert(tm_tid >= 0);

	sns.snr = SN_SUBSCRIBE;

	while(true){
		Send(pub_tid, &sns, sizeof(sns), sensors, sizeof(sensors));
		// TMLogStrf(tm_tid, "Sensors:%d%d %d%d %d%d %d%d %d%d\n", sensors[0],sensors[1],sensors[2],sensors[3],sensors[4],sensors[5],sensors[6],sensors[7],sensors[8],sensors[9]);
	}

	Exit();
}

void SensorProvider(){
	tid_t tid_req;
  	SNProtocol snp;

	int scounter = 0;
	int reply;
  	bool recFlag = false;
  	bool courierFlag = false;
  	bool dataFlag = false;

  	char sensors[DECODER_SIZE*2];

    tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
    assert(tx_tid >= 0);

    Create(27, &SensorReceiver);
  	Create(27, &SensorTimeout);
  	Create(27, &SensorPublisher);
    tid_t suc_tid = Create(27, &SensorUpdateCourier);
   	// Create(19, &TestSNPublisher);

  	//Kick start sensor gathering data
  	BLPutC(tx_tid, GET_ALL_SENSORS);
  	while(true){
  		//If courier is ready, send the data
  		if(dataFlag && courierFlag){
  			dataFlag = false;
  			courierFlag = false;
  			Reply(suc_tid, &sensors, sizeof(sensors));
  		}

  		Receive(&tid_req, &snp, sizeof(snp));

  		switch(snp.snr){
  			case SN_READBYTE:
  				Reply(tid_req, &reply, sizeof(reply));
  				sensors[scounter] = snp.byte;
  				scounter = (scounter + 1) % (DECODER_SIZE*2);
  				if(scounter == 0){
  					BLPutC(tx_tid, GET_ALL_SENSORS);
  					dataFlag = true;
  				}
  				if(scounter % 2 == 0){
  					recFlag = true;
  				}
  				break;
  			case SN_CHECK:
  				Reply(tid_req, &recFlag, sizeof(recFlag));
  				recFlag = false;
  				break;
  			case SN_RESET:
  				Reply(tid_req, &reply, sizeof(reply));
  				recFlag = false;
  				scounter = 0;
  				BLPutC(tx_tid, GET_ALL_SENSORS);
  				break;
  			case SN_NOTIFY_READY:
  				courierFlag = true;
  				break;
  			case SN_HALT:
  				Reply(tid_req, &reply, sizeof(reply));
  				break;
  			default:
  				assert(0 && "Bad Sensor Command");
  		}
  	}

  	Exit();
}
