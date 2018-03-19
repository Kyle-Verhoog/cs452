#ifndef TRACK_H
#define TRACK_H

#include <lib/circular_buffer.h>

#ifndef X86
#include <user/syscalls.h>
#else
#include <assert.h>
#endif

#include <lib/train/events.h>
#include <lib/train/train_defines.h>
#include <lib/train/track_node.h>
#include <lib/train/track_data.h>
#include <lib/train/updates.h>
#include <lib/train/sensor.h>
#include <lib/train/switch.h>
#include <lib/train/train.h>

#define TRAIN_LIST_SIZE 2
CIRCULAR_BUFFER_DEC(train_cb, Train, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEC(train_list, Train *, TRAIN_LIST_SIZE);
#define UPDATE_LIST_SIZE 8
CIRCULAR_BUFFER_DEC(update_list, TrackEvent, UPDATE_LIST_SIZE);
#define VEVENT_LIST_SIZE 8
CIRCULAR_BUFFER_DEC(ve_list, VirtualEvent, VEVENT_LIST_SIZE);

#define POSSIBLE_NODE_LIST_SIZE 5
CIRCULAR_BUFFER_DEC(poss_node_list, track_node *, POSSIBLE_NODE_LIST_SIZE);

typedef struct Track {
  track_node *graph;
  Train train[TRAIN_SIZE];
  train_list lost_trains;
  train_list active_trains;
  Sensor sensors[DECODER_SIZE*8];
  Switch switches[SWITCH_SIZE];
  update_list updates;
  ve_list vevents;
  // TODO: model updates
} Track;

void TrackInit(Track *, track_node *graph);

Train *GetActiveTrain(Track *track, int train_num);

Train *RemoveActiveTrain(Track *track, int train_num);

void TrackAddLostTrain(Track *track, Train *t);

int TrackGetNextPossibleSensors(Track *track, track_node *node, poss_node_list *pnl);

int TrackInterpretEventGroup(Track *track, EventGroup *grp);
#endif