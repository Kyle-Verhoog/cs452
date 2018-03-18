#ifndef WAITING_ROOM_H
#define WAITING_ROOM_H

#include <defines.h>
#include <train_defines.h>
#include <user/clockserver.h>
#include <user/train/events.h>
#include <user/train/virtual_provider.h>
#include <sensor_provider.h>
#include <train_provider.h>
#include <switch_provider.h>

typedef enum EventGroupType {
  VRE_VE_RE,
  VRE_VE,
  VRE_RE,
  RE,
} EventGroupType;


typedef struct EventGroup {
  EventGroupType type;
  RawEvent       re;
  VirtualEvent   ve;
} EventGroup;

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

#endif
