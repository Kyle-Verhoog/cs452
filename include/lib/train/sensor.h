#ifndef SENSOR_H
#define SENSOR_H

typedef enum SensorState{
	SEN_OFF = 0,
	SEN_ON = 1
}SensorState;

typedef struct Sensor{
	SensorState state;
	track_node *node;
}Sensor;

typedef enum SN_Request{
	//Manager Reqs
	SN_HALT = -1,
	SN_READBYTE = 0,
	SN_CHECK = 1,
	SN_RESET = 2,
	//User Reqs
	SN_NOTIFY_READY = 3,
	SN_NOTIFY = 4,
	SN_SUBSCRIBE = 5,
	SN_SUBSCRIBE_DELTA = 6,
	SN_GET_ALL = 10
} SN_Request;

typedef struct SNProtocol{
	SN_Request snr;
	char byte;
}SNProtocol;

typedef struct RawSensors {
  char sensors[DECODER_SIZE * 2];
} RawSensors;

typedef struct SNSubscribe{
	SN_Request snr;
  RawSensors sensors;
}SNSubscribe;

#endif //SENSOR_H
