#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/clockserver.h>
#include <user/train/representer.h>
#include <user/train/train_provider.h>
#include <lib/train/updates.h>
#include <lib/train/track_switch.h>

#define MEASURING_GEAR 4

typedef struct CalibrationArgs{
	int train;
	int init_gear;
	int target_node;
}CalibrationArgs;

typedef struct TestCalibArgs{
	int train;
	int gear;
	int dist;
	int target_node;
}TestCalibArgs;

typedef struct ATestCalibArgs{
	int train;
	int gear;
	int delay;
	int dist;
}ATestCalibArgs;

void Calibration(void *args);
void AccelCalibration(void *args);
void SetMS(void *args);
void MeasuringVelocity(void *args);
void TestCalibration(void *args);
void ATestCalibration(void *args);

#endif //CALIBRATION_H
