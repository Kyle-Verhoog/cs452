#ifndef STOPPING_CALIBRATION_TEST_H
#define STOPPING_CALIBRATION_TEST_H
#include <io.h>
#include <ts7200.h>
#include <user/syscalls.h>

#include <train_provider.h>
#include <switch_provider.h>
#include <sensor_provider.h>
#include <prediction_manager.h>
#include <lib/train/path_finding.h>

#define INCHING_GEAR 5

/*

typedef struct StopReference{
	track_node *ref;
	track_node *target;
	int delayDist;
}StopReference;

typedef struct InchingArgs{
	tid_t tm_tid;
	tid_t sm_tid;
	tid_t cs_tid;
	tid_t my_tid;
	Switch *switches;
	TrainDescriptor *train;
}InchingArgs;

typedef struct StoppingCalibrationArgs{
	int train;
	int speed;
	int start_node;
	int before_sensor;
}StoppingCalibrationArgs;

StopReference StopOverPath(TrainDescriptor *td, int stopDist);

void InitInch(InchingArgs args);
int InchForward(InchingArgs args);

void StoppingCalibrationTest(void *args);
void StoppingServerTest();
*/

#endif
