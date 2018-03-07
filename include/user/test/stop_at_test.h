#ifndef STOP_AT_TEST_H
#define STOP_AT_TEST_H
#include <io.h>
#include <ts7200.h>
#include <user/syscalls.h>

#include <railway_manager.h>
#include <train_manager.h>
#include <switch_manager.h>
#include <sensor_manager.h>
#include <prediction_manager.h>

#define INCHING_GEAR 5

typedef enum StopAtReq{
	SAR_STOP = 0,
}StopAtReq;

typedef struct StopAtProtocol{
	StopAtReq sar;
	int train;
	int gear;
	int stop_at;
	int stop_from;
	int start;
}StopAtProtocol;

typedef struct InchingArgs{
	tid_t tm_tid;
	tid_t sm_tid;
	tid_t cs_tid;
	tid_t my_tid;
	Switch *switches;
	TrainDescriptor *train;
}InchingArgs;

void InitInch(InchingArgs args);
void StopAt(void *args);
void StopAtServer();

#endif
