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

typedef enum SM_Request{
	//Manager Reqs
	SM_HALT = -1,
	SM_READBYTE = 0,
	SM_CHECK = 1,
	SM_RESET = 2,
	//User Reqs
	SM_NOTIFY_READY = 3,
	SM_NOTIFY = 4,
	SM_SUBSCRIBE = 5,
	SM_SUBSCRIBE_DELTA = 6,
	SM_GET_ALL = 10
} SM_Request;

typedef struct SMProtocol{
	SM_Request smr;
	char byte;
}SMProtocol;

typedef struct SMSubscribe{
	SM_Request smr;
	char sensors[DECODER_SIZE * 2];
}

#endif //SENSOR_H
