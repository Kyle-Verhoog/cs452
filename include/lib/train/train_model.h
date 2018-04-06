#ifndef TRAIN_MODEL_H
#define TRAIN_MODEL_H

#define BITSPERINT 32
#define TOP2BITS(x) ((x & (3L << (BITSPERINT-2))) >> (BITSPERINT-2))

#define TRAIN_MODEL_SIZE 15
#define MEASURING_GEAR 4

#ifdef X86
	#include <assert.h>
#endif
#ifndef X86
	#include <syscalls.h>
#endif

typedef struct TrainModel{
	int train_num;
	int x[TRAIN_MODEL_SIZE];
	int y[TRAIN_MODEL_SIZE];
	int t[TRAIN_MODEL_SIZE];
}TrainModel;

typedef struct TrainModelSnapshot{
	int cur_gear;
	int start_gear;
	int end_gear;
	int duration;		//Time since start_gear
	int elapsed;		//Time delta
	TrainModel model;
} TrainModelSnapshot;

//Integer Square Root
unsigned int usqrt(int x);

//Assumes integer division
int mean(int * list, int size);
int standard_deviation(int *list, int size);

//Get Values from Lagrange Form
//NOTE: TO AVOID FLOATING POINT, INTERPOLATE MULTIPLIES GEAR SETTING BY 10
int interpolate(TrainModel *tm, int setting);
int interpolatePartial(TrainModel *tm, int setting, int head, int tail);
int easyInterpolation(TrainModel *tm, int setting);

//NOTE: ONLY USE THIS IF YOU ARE SURE LINEAR CONTINUITY
int estimateGear(int *gear, int *model, int point);
//Compute Dist train has travelled over time
int trainUpdateDist(TrainModelSnapshot* tms, int train_num);

void getStoppingDistanceModel(TrainModel *tm, int train_num);
void getVelocityModel(TrainModel *tm, int train_num);
void getAccelerationDistanceModel(TrainModel *tm, int train_num);

//Update Values
#define ACCEPTED 0
#define REJECTED 1
#define ALPHA 50
int alphaUpdate(TrainModel *tm, int setting, int velocity);

void getVelocityModel(TrainModel *tm, int train_num);
#endif //TRAIN_MODEL_H
