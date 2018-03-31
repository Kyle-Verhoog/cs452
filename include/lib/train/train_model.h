#ifndef TRAIN_MODEL_H
#define TRAIN_MODEL_H

#define BITSPERINT 32
#define TOP2BITS(x) ((x & (3L << (BITSPERINT-2))) >> (BITSPERINT-2))

#define TRAIN_MODEL_SIZE 15

#ifdef X86
	#include <assert.h>
#endif
#ifndef X86
	#include <syscalls.h>
#endif

typedef struct TrainModel{
	int x[TRAIN_MODEL_SIZE];
	int y[TRAIN_MODEL_SIZE];
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

#endif //TRAIN_MODEL_H
