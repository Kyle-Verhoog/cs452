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

void getStoppingDistanceModel(TrainModel *tm, int train_num);
void getVelocityModel(TrainModel *tm, int train_num);
void getAccelerationDistanceModel(TrainModel *tm, int train_num);


//Update Values
#define ACCEPTED 0
#define REJECTED 1
#define ALPHA 50
int alphaUpdate(TrainModel *tm, int setting, int velocity);


// TRAIN DATA
static TrainModel TR_77_VELO = {
	77,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 100, 410, 870, 1340, 1360, 1670, 2220, 2712, 3381, 4063, 4785, 5337, 5936, 6728}
};

static TrainModel TR_77_STP_DIST = {
	77,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 0, 13564, 34176, 94045, 168608, 277890, 402523, 570764, 730057, 954537, 1215350},
	{0, 0, 0, 0, 0, 20, 54, 84, 128, 156, 188, 224, 271, 294, 336}
};

static TrainModel TR_77_ACCEL_DIST = {
	77,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 0, 25580, 88240, 184186, 296202, 396204, 528630, 650541, 831305, 1014943, 1257763},
	{0, 0, 0, 0, 0, 50, 115, 185, 259, 292, 331, 357, 381, 420, 440}
};

//////////////////////////////////



#endif //TRAIN_MODEL_H
