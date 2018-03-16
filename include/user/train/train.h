#ifndef TRAIN_H
#define TRAIN_H

#include <types.h>
#include <lib/train/track_node.h>
#include <lib/train/path_finding.h>
#include <user/train/train_defines.h>

#define TRAIN_COMMAND_BUFFER_SIZE 64

/****************
Train Manager Commands

MOVE 		- <Train> <Speed>
REVERSE 	- <Train>
DELAY		- <Train> <Duration>
TSKCOMPL	- <Train>

****************/

/****************
Train Commands

MOVE 		- <Train> <Speed>
REVERSE 	- <Train>
DELAY		- <Duration>

****************/

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


typedef enum Train_Command{
  T_MOVE = 0,
  T_REVERSE = 1,
  T_DELAY = 2,
  T_INIT = 3,
}Train_Command;

typedef struct TrainProtocol{
  Train_Command tc;
  char arg1;
  char arg2;
}TrainProtocol;

typedef struct TMProtocol{
  TM_Command tmc;
  char arg1;
  char arg2;
}TMProtocol;

typedef struct TMSubscribe{
	TM_Command tmc;
	TrainProtocol tp;
}TMSubscribe;


CIRCULAR_BUFFER_DEC(tc_cb, TrainProtocol, TRAIN_COMMAND_BUFFER_SIZE);

typedef struct Train {
  int id;
  int gear;
  int dir; //forward is 1
  bool exist;
  track_node *pos;
  int speed;
  int time_stamp;
} Train;

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
