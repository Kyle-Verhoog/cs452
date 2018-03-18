#ifndef TRACK_H
#define TRACK_H

#include <lib/circular_buffer.h>

#include <user/syscalls.h>
#include <lib/train/train_defines.h>
#include <lib/train/track_node.h>
#include <lib/train/track_data.h>
#include <lib/train/sensor.h>
#include <lib/train/switch.h>
#include <lib/train/train.h>

#define TRAIN_LIST_SIZE 2

CIRCULAR_BUFFER_DEC(train_cb, Train, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEC(train_list, Train *, TRAIN_LIST_SIZE);

#define POSSIBLE_NODE_LIST_SIZE 5
CIRCULAR_BUFFER_DEC(poss_node_list, track_node *, POSSIBLE_NODE_LIST_SIZE);

typedef struct Track {
  train_cb all_trains;
  train_list orphan_trains;
  train_list active_trains;
  char sensors[DECODER_SIZE*2];
  // Sensor sensors[DECODER_SIZE*8];
  Switch switches[SWITCH_SIZE];
  // trains..
  //  - locations
  //  - destinations
  //  - paths
  //  - speed information
  // switch configurations
} Track;

void TrackInit(Track *);

Train *GetActiveTrain(Track *track, int train_num);

Train *RemoveActiveTrain(Track *track, int train_num);

int TrackGetNextPossibleSensors(Track *track, track_node *node, poss_node_list *pnl);
#endif
