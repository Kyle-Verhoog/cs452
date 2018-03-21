#ifndef UTRAIN_H
#define UTRAIN_H

#include <types.h>
#include <lib/train/track_node.h>
#include <lib/train/path_finding.h>
#include <lib/train/train_defines.h>

#define TRAIN_COMMAND_BUFFER_SIZE 64

/* ========DEPRECATED========= */

typedef enum TM_Command{
  TM_MOVE = 0,
  TM_REVERSE = 1,
  TM_DELAY = 2,
  TM_TASK_COMPLETE = 3,
  TM_TRACK = 4,
  TM_MEASURE = 5,
  TM_GET_ALL = 6,
  //USER COMMANDS
  TM_NOTIFY = 10,
  TM_SUBSCRIBE = 11
}TM_Command;

typedef struct TMProtocol{
  TM_Command tmc;
  char arg1;
  char arg2;
}TMProtocol;


CIRCULAR_BUFFER_DEC(tc_cb, TrainProtocol, TRAIN_COMMAND_BUFFER_SIZE);

typedef struct TrainDescriptor{
  int id;
  int gear;
  bool dir; //forward is 1
  bool exist;

  tid_t isRunning;
  tc_cb buf;

  track_node *node;
  int speed;
  int inchingSpeed;
  int time_of_sensor;
  path tpath;
} TrainDescriptor;

#endif
