#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/clockserver.h>
#include <user/train/representer.h>
#include <user/train/train_provider.h>

#define MEASURING_GEAR 4

typedef struct CalibrationArgs{
	int train;
	int init_speed;
	int target_node;
}CalibrationArgs;

void Calibration(void *args);
void MeasuringVelocity();

#endif //CALIBRATION_H