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
#include <user/train/events.h>

typedef struct TrainSubscribe{
	Train_Command tc;
	RawEvent re;
}TSubscribe;

void TrainProvider();

#endif //TRAIN_PROVIDER_H
