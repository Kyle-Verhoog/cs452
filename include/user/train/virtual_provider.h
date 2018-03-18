#ifndef VIRTUAL_PROVIDER_H
#define VIRTUAL_PROVIDER_H

#include <lib/train/train_defines.h>
#include <user/train/events.h>
#include <tid_buffer.h>

#define KEY_SIZE SENSOR_SIZE

typedef enum VirtualEventRequestType{
	VER_REGISTER,
	VER_EVENT,
	//User Reqs
	VER_NOTIFY_READY,
	VER_NOTIFY,
	VER_SUBSCRIBE
}VirtualEventRequestType;

typedef struct VERequest{
	VirtualEventRequestType type;
	VirtualEvent ve;
}VERequest;

typedef struct VESubscribe{
	VirtualEventRequestType type;
	VERequest req;
}VESubscribe;


void VirtualProvider();

#define VEREQ_BUFFER_SIZE 64

CIRCULAR_BUFFER_DEC(vereq_cb, VERequest, VEREQ_BUFFER_SIZE);
#endif
