#ifndef TRAIN_MODEL_H
#define TRAIN_MODEL_H

#define BITSPERINT 32
#define TOP2BITS(x) ((x & (3L << (BITSPERINT-2))) >> (BITSPERINT-2))

#define TRAIN_MODEL_SIZE 15

typedef struct TrainModel{
	int x[15];
	int y[15];
}

//Integer Square Root
int sqrt(int x);

//Assumes integer division
int mean(int * list, int size);
int standard_deviation(int *list, int size);


#endif //TRAIN_MODEL_H