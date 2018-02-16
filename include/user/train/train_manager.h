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

typedef enum TM_Command{
	TM_MOVE = 0,
	TM_REVERSE = 1,
}TM_Command;

typedef struct TMProtocol{
	TM_Command tmc;
	char arg1;
	char arg2;
}TMProtocol;

void TrainManager();

#endif
