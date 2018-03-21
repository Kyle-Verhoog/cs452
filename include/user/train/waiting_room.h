#ifndef WAITING_ROOM_H
#define WAITING_ROOM_H

#include <defines.h>
#include <train_defines.h>
#include <lib/train/events.h>
#include <user/clockserver.h>
#include <user/train/virtual_provider.h>
#include <sensor_provider.h>
#include <train_provider.h>
#include <switch_provider.h>

typedef enum WRRequestType {
  WR_VE,
  WR_RE,
  WR_CE,
  WR_TO,
} WRRequestType;

union WRRequestData {
  VirtualEvent ve;
  RawEvent     re;
};

typedef struct WRRequest {
  WRRequestType type;
  union WRRequestData data;
} WRRequest;

void WaitingRoom();

#define EVENT_GROUP_BUFFER_SIZE 32
#define VE_KEY_BUFFER_SIZE 8

CIRCULAR_BUFFER_DEC(eg_cb, EventGroup, EVENT_GROUP_BUFFER_SIZE);
CIRCULAR_BUFFER_DEC(ve_key_cb, int, VE_KEY_BUFFER_SIZE);

#endif
