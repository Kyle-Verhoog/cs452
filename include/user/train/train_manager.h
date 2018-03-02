#ifndef TRAIN_MANAGER_H
#define TRAIN_MANAGER_H

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
#include <track_data.h>

#define TRAIN_COMMAND_BUFFER_SIZE 64

typedef enum TM_Command{
	TM_MOVE = 0,
	TM_REVERSE = 1,
	TM_DELAY = 2,
	TM_TASK_COMPLETE = 3
}TM_Command;

typedef struct TMProtocol{
	TM_Command tmc;
	char arg1;
	char arg2;
}TMProtocol;

typedef enum Train_Command{
	T_MOVE = 0,
	T_REVERSE = 1,
	T_DELAY = 2
}Train_Command;

typedef struct TrainProtocol{
	Train_Command tc;
	char arg1;
	char arg2;
}TrainProtocol;

//========================================================//

CIRCULAR_BUFFER_DEC(tc_cb, volatile TrainProtocol, TRAIN_COMMAND_BUFFER_SIZE);

#define INIT_TRAIN(td, train_id) td.id = train_id; \
	td.speed = 0; \
	td.dir = 1; \
	td.isRunning = -1; \
	tc_cb_init(&td.buf); \

typedef struct TrainDescriptor{
	int id;
	int speed;
	bool dir;	//forward is 1

	tid_t isRunning;
	tc_cb buf;
} TrainDescriptor;

void TMWriteTask(void *args);	//args - TrainDescriptor
void TrainManager(void * args);


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


#endif //TRAIN_MANAGER_H
