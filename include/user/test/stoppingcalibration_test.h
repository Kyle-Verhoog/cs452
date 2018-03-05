#ifndef STOPPING_CALIBRATION_TEST_H
#define STOPPING_CALIBRATION_TEST_H
#include <io.h>
#include <ts7200.h>
#include <user/syscalls.h>

#include <railway_manager.h>
#include <train_manager.h>
#include <switch_manager.h>
#include <sensor_manager.h>
#include <prediction_manager.h>


typedef struct StoppingCalibrationArgs{
	int train;
	int speed;
	int start_node;
	int before_sensor;
}StoppingCalibrationArgs;

void StoppingCalibrationTest(void *args);
void StoppingServerTest();

#endif
