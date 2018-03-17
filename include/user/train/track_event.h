#ifndef TRACK_EVENT_H
#define TRACK_EVENT_H

#include <user/train/train.h>
#include <lib/train/switch.h>
#include <lib/train/sensor.h>

#define MAX_NUM_UPDATES 10

typedef enum RawTrackEventType {
  RTE_SE = 0,
  RTE_SW = 1,
  RTE_TR_CMD  = 2,
} RawTrackEventType;

typedef Sensors RawSensorEvent;
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


typedef enum TrackEventType {
  TE_TR_MOVE,
  TE_SW_CHANGE,
  TE_SE_CHANGE,
  MAX_TRACK_EVENT  // NOTE: keep as last element in enum
} TrackEventType;


typedef struct TrackEventTrainPositionChange {
} TETRPosition;

typedef struct TrackEventTrainGearChange {
  int num;
  int newgear;
} TETRGear;

typedef struct TrackEventSwitchChange {
  int num;
  int newdir;
} TESWChange;

typedef struct TrackEventSensorChange {
  int dec;
  int sen;
} TESEChange;

union TrackEvents {
  TETRPosition tr_pos;
  TETRGear tr_gear;
  TESWChange sw_change;
  TESEChange se_change;
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
