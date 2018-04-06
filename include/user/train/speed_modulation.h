#ifndef SPEED_MODULATION_H
#define SPEED_MODULATION_H

#include <lib/train/train_defines.h>
#include <user/train/train_provider.h>
// #include <lib/train/train_model.h>
#include <defines.h>

#define CONTINUE 0
#define SUSPEND 1
#define EXIT 2

#define MOD_INTERVAL 20

typedef enum ModulationRequest{
	MR_MOD_REQ,
	MR_SUSPEND_REQ,
	MR_QUIT_REQ,
	//FOR Speed Modulator Use
	MR_REPORT_RES
}ModulationRequest;

typedef struct Modulator{
	ModulationRequest mr;
	int train_num;
	int velocity;
}Modulator;

void SpeedModulator(void *args);
void SpeedModulationService();

#endif //SPEED_MODULATION_H
