#ifndef STOP_AT_TEST_H
#define STOP_AT_TEST_H
#include <io.h>
#include <ts7200.h>
#include <user/syscalls.h>
#include <tid_buffer.h>
#include <train_manager.h>
#include <switch_manager.h>
#include <sensor_manager.h>
#include <prediction_manager.h>

#include <stoppingcalibration_test.h>

#define STOP_PATH_SIZE 128

typedef enum StopAtReq{
	SAR_STOP = 0,
}StopAtReq;

typedef struct StopAtProtocol{
	StopAtReq sar;
	int train;
	int gear;
	int stop_at;
	int dist;
}StopAtProtocol;

void StopAt(void *args);
void StopAtServer();

#endif
