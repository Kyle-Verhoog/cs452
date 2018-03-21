#ifndef TRAIN_H
#define TRAIN_H

#include <types.h>
#include <lib/train/track_node.h>
#include <lib/train/event_window.h>

#define IS_VALID_TR_NUM(x) (x >= 1 && x <= 81)

typedef enum TrainStatus {
  TR_UNINIT,   // uninitialized (has not been added to track)
  TR_LOST,     // unknown position and speed
  TR_UN_SPEED, // unknown speed, known position
  TR_KNOWN,    // known position and speed
} TrainStatus;


typedef struct Train {
  int num;
  int gear;
  int dir; //forward is 1
  event_window window;
  track_node *pos;
  int speed;
  int ts;
  int sen_ts;     // timestamp of last sensor
  TrainStatus status;
} Train;

#endif
