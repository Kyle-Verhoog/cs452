#ifndef UPDATES_H
#define UPDATES_H

#define MAX_NUM_UPDATES 10

typedef enum TrackEventType {
  TE_TR_MOVE,
  TE_TR_POSITION,
  TE_SW_CHANGE,
  TE_SE_CHANGE,
  MAX_TRACK_EVENT  // NOTE: keep as last element in enum
} TrackEventType;

typedef enum TrackDataType{
  TD_ALL,
  TD_TR_A,
  TD_TR_L,
  TD_SW,
  TD_SE
} TrackDataType;


typedef struct TrackEventTrainPositionChange {
  int num;
  int node;
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
  int num;
  int state;
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
