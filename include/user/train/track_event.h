#ifndef TRACK_EVENT_H
#define TRACK_EVENT_H

#include <user/train/train.h>
#include <lib/train/switch.h>
#include <lib/train/sensor.h>

#define MAX_NUM_UPDATES 10

typedef enum RawTrackEventType {
  RTE_SENSOR = 0,
  RTE_SWITCH = 1,
  RTE_TRAIN  = 2,
} RawTrackEventType;

typedef SMProtocol RawSensorEvent;
typedef SWProtocol RawSwitchEvent;
typedef TrainProtocol RawTrainCmdEvent;

union RawTrackEvents {
  RawTrainCmdEvent  tr_cmd_event;
  RawSensorEvent    se_event;
  RawSwitchEvent    sw_event;
};

typedef struct RawTrackEvent {
  RawTrackEventType type;
  union RawTrackEvents event;
} RawTrackEvent;


#define MAX_TRACK_EVENT 1
typedef enum TrackEventType {
  TE_TRAIN_MOVE = 0,
} TrackEventType;


typedef struct TrackEventTrainPositionChange {
} TETRPosition;

typedef struct TrackEventTrainGearChange {
  int num;
  int newgear;
} TETRGear;

typedef struct TrackEventSwitchChange {
} TESWChange;

union TrackEvents {
  TETRPosition tr_pos;
  TETRGear tr_gear;
  TESWChange sw_toggle;
};

typedef struct TrackEvent {
  TrackEventType type;
  union TrackEvents event;
} TrackEvent;

typedef struct TrackUpdate {
  TrackEvent events[MAX_NUM_UPDATES];
  int num;
} TrackUpdate;

#endif
