#ifndef TRAIN_PROVIDER_H
#define TRAIN_PROVIDER_H

#include <ts7200.h>
#include <defines.h>
#include <train_defines.h>
#include <user/syscalls.h>
#include <io.h>
#include <ascii.h>
#include <ioserver.h>
#include <writerservice.h>
#include <types.h>
#include <circular_buffer.h>
#include <lib/train/path_finding.h>
#include <tid_buffer.h>
#include <user/terminal_manager.h>

#define TRAIN_COMMAND_BUFFER_SIZE 64

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

typedef struct TMSubscribe{
	TM_Command tmc;
	TrainProtocol tp;
}TMSubscribe;


//========================================================//

CIRCULAR_BUFFER_DEC(tc_cb, volatile TrainProtocol, TRAIN_COMMAND_BUFFER_SIZE);

typedef struct TrainDescriptor{
	int id;
	int gear;
	bool dir;	//forward is 1
	bool exist;

	tid_t isRunning;
	tc_cb buf;

	//Modifiable By Model
	track_node *node;
	int speed;
	int inchingSpeed;
	int time_of_sensor;
  path tpath;
} TrainDescriptor;

void TrainProvider();

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


#endif //TRAIN_PROVIDER_H
