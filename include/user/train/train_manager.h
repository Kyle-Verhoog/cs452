#ifndef TRAIN_MANAGER_H
#define TRAIN_MANAGER_H

#include <ts7200.h>
#include <defines.h>
#include <train_defines.h>
#include <user/syscalls.h>
#include <io/io.h>
#include <ascii.h>
#include <ioserver.h>
#include <writerservice.h>
#include <readerservice.h>

typedef struct TMProtocol{
	TR_Command trc;
	int arg1;
	int arg2;
}TMProtocol;

void TrainManager();

#endif
