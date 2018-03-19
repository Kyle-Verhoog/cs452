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

#endif
