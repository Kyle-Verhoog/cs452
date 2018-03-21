#ifndef TRAIN_H
#define TRAIN_H

#include <types.h>
#include <lib/train/track_node.h>
#include <lib/train/event_window.h>
#include <lib/train/train_defines.h>

#define IS_VALID_TR_NUM(x) (x >= 1 && x <= 81)

/****************
Train Manager Commands

MOVE    - <Train> <Speed>
REVERSE   - <Train>
DELAY   - <Train> <Duration>
TSKCOMPL  - <Train>

****************/

/****************
Train Commands

MOVE    - <Train> <Speed>
REVERSE   - <Train>
DELAY   - <Duration>

****************/

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

typedef enum Train_Command{
  T_MOVE = 0,
  //USER COMMANDS
  T_NOTIFY = 10,
  T_SUBSCRIBE = 11
}Train_Command;

typedef struct TrainProtocol{
  Train_Command tc;
  char arg1;
  char arg2;
}TrainProtocol;

#endif
