#ifndef DRIVER_H
#define DRIVER_H

#include <types.h>
#include <lib/train/track_node.h>

typedef struct TrainDriverArgs {
  int train_num;
  track_node *end;
} TrainDriverArgs;

extern bool DRIVER1_DEF, DRIVER2_DEF;

void TrainDriver();

#endif
