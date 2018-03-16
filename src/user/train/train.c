#include <user/train/train.h>

CIRCULAR_BUFFER_DEF(tc_cb, TrainProtocol, TRAIN_COMMAND_BUFFER_SIZE);
