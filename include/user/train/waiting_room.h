#ifndef WAITING_ROOM_H
#define WAITING_ROOM_H

#include <user/clockserver.h>
#include <user/train/events.h>
#include <user/train/virtual_provider.h>

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

#endif
