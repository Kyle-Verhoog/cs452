#ifndef TRACK_H
#define TRACK_H

#include <lib/circular_buffer.h>

#ifndef X86
#include <user/syscalls.h>
#else
#include <assert.h>
#define assertf(cond, ...) assert(cond)
#endif

#include <lib/train/events.h>
#include <lib/train/train_defines.h>
#include <lib/train/track_node.h>
#include <lib/train/track_data.h>
#include <lib/train/updates.h>
#include <lib/train/sensor.h>
#include <lib/train/switch.h>
#include <lib/train/train.h>
#include <lib/train/path_finding.h>


#define TRAIN_LIST_SIZE 2
EXT_CIRCULAR_BUFFER_DEC(train_cb, Train, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEC(train_list, Train *, TRAIN_LIST_SIZE);
#define UPDATE_LIST_SIZE 16
CIRCULAR_BUFFER_DEC(update_list, TrackEvent, UPDATE_LIST_SIZE);
#define VEVENT_LIST_SIZE 16
CIRCULAR_BUFFER_DEC(ve_list, VirtualEvent, VEVENT_LIST_SIZE);

#define BFS_Q_SIZE 16
CIRCULAR_BUFFER_DEC(bfs_q, track_node*, BFS_Q_SIZE);

typedef struct PossibleSensor {
  int dist;
  track_node *node;
} PossibleSensor;

#define POSSIBLE_NODE_LIST_SIZE 5
CIRCULAR_BUFFER_DEC(poss_node_list, PossibleSensor, POSSIBLE_NODE_LIST_SIZE);

#define KEY_SIZE 256
#define NUM_TRAINS 5

typedef struct Track {
  track_node *graph;
  Train train[NUM_TRAINS];
  int tmap[TRAIN_SIZE];
  int ntrains;
  train_list lost_trains;
  train_list active_trains;
  Sensor sensors[DECODER_SIZE*8];
  Switch switches[SWITCH_SIZE];
  update_list updates;
  ve_list vevents;
  int key;                   // key unique to each event
} Track;

void TrackInit(Track *, track_node *graph);

Train *GetActiveTrain(Track *track, int train_num);

Train *RemoveActiveTrain(Track *track, int train_num);

void TrackAddTrain(Track *track, Train *t);

int GetNextPossibleSensors(track_node *node, int dist, poss_node_list *pnl);

int GetPrevSensorINC(Switch *sw, track_node *n, PossibleSensor *pos); //Inclusive, Overshoots distance
int GetPrevSensorEXC(Switch *sw, track_node *n, PossibleSensor *pos); //Exclusive, Undershoots distance
int GetNextSensorINC(Switch *sw, track_node *n, PossibleSensor *pos); //Inclusive, Overshoots distance
int GetNextSensorEXC(Switch *sw, track_node *n, PossibleSensor *pos); //Exclusive, Undershoots distance
int DistanceBetweenNodes(Switch *sw, track_node *start, track_node *end);

int TrackInterpretEventGroup(Track *track, EventGroup *grp);
#endif
