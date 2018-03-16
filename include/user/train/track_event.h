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

// TODO: these protocols should be kept here
typedef SMProtocol RawSensorEvent;
typedef SWProtocol RawSwitchEvent;
typedef TrainProtocol RawTrainEvent;

union RawTrackEvents {
  RawTrainEvent  tr_event;
  RawSensorEvent se_event;
  RawSwitchEvent sw_event;
};

typedef struct RawTrackEvent {
  RawTrackEventType type;
  union RawTrackEvents event;
} RawTrackEvent;


typedef enum TrackEventType {
  TE_TRAIN_MOVE = 0,
} TrackEventType;


typedef struct TrackEventTrainPositionChange {
} TETRPosition;

typedef struct TrackEventTrainSpeedChange {
} TETRSpeed;

typedef struct TrackEventSwitchChange {
} TESWChange;

union TrackEvents {
  TETRPosition tr_pos;
  TETRSpeed tr_speed;
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
