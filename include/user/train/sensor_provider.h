#ifndef SNESOR_PROVIDER_H
#define SNESOR_PROVIDER_H

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
#include <events.h>

#define GET_ALL_SENSORS 133

#define SENSOR_TIMEOUT 4
#define SENSOR_WAIT 4

typedef struct SNSubscribe{
	SN_Request snr;
	RawEvent re;
}SNSubscribe;

void SensorProvider();

#endif //SNESOR_PROVIDER_H
