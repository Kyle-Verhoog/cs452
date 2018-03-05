#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <ts7200.h>
#include <defines.h>
#include <train_defines.h>
#include <user/syscalls.h>
#include <io.h>
#include <ascii.h>
#include <ioserver.h>
#include <i2a.h>
#include <writerservice.h>
#include <recency_buffer.h>
#include <track_data.h>
#include <tid_buffer.h>
#include <user/ui/sensor_interface.h>

#define GET_ALL_SENSORS 133

#define SENSOR_TIMEOUT 4
#define SENSOR_WAIT 4

typedef enum SensorState{
	SEN_OFF = 0,
	SEN_ON = 1
}SensorState;

typedef enum SM_Request{
	SM_HALT = -1,
	SM_READBYTE = 0,
	SM_CHECK = 1,
	SM_RESET = 2,
	SM_SUBSCRIBE = 3,
	SM_SUBSCRIBE_DELTA = 4,
	SM_GET_ALL = 10
} SM_Request;

typedef struct Sensor{
	SensorState state;
	track_node *node;
}Sensor;

typedef struct SMProtocol{
	SM_Request smr;
	char byte;
}SMProtocol;

void SensorManager(void * args);

#endif //SENSOR_MANAGER_H
