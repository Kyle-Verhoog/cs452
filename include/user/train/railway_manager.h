#ifndef RAILWAY_MANAGER_H
#define RAILWAY_MANAGER_H

#include <ts7200.h>
#include <defines.h>
#include <train_defines.h>
#include <user/syscalls.h>
#include <io.h>
#include <ascii.h>
#include <ioserver.h>
#include <writerservice.h>
#include <lib/parse.h>
#include <lib/train/track_data.h>

//Train Servers
#include <switch_manager.h>
#include <train_manager.h>
#include <sensor_manager.h>
#include <prediction_manager.h>

typedef enum RW_Command{
	RW_QUIT = -1,
	RW_TRAIN = 1,
	RW_REVERSE = 2,
	RW_SWITCH = 3,
	RW_SENSOR = 4,
	RW_TRACK = 5,
	RW_GET_NODE = 6,
	RW_GET_TRACK = 7
} RW_Command;

typedef struct RWProtocol{
	RW_Command rwc;
	char *command;
	int size;
}RWProtocol;

void RailwayManager();

#endif
