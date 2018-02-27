#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <ts7200.h>
#include <defines.h>
#include <train_defines.h>
#include <user/syscalls.h>
#include <io.h>
#include <ascii.h>
#include <ioserver.h>
#include <writerservice.h>
#include <recency_buffer.h>

#define GET_ALL_SENSORS 133

#define SENSOR_TIMEOUT 0
#define SENSOR_WAIT 1

typedef enum Sensor{
	SEN_OFF = 0,
	SEN_ON = 1
}Sensor;

typedef enum SM_Request{
	SM_READBYTE = 0,
	SM_CHECK = 1,
	SM_RESET = 2,
	SM_HALT = 3
} SM_Request;

typedef struct SMProtocol{
	SM_Request smr;
	char byte;
}SMProtocol;

void SensorManager();

#endif //SENSOR_MANAGER_H
