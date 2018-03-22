#ifndef DRIVER_H
#define DRIVER_H

#include <types.h>

typedef struct TrainDriverArgs {
  int train_num;
  int start;
  int end;
} TrainDriverArgs;

extern bool DRIVER1_DEF, DRIVER2_DEF;

void TrainDriver();

#endif
