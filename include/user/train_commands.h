#ifndef TRAIN_CMDS_H
#define TRAIN_CMDS_H

#include <defines.h>
#include <lib/parse_args.h>
#include <lib/train/track_data.h>
#include <user/syscalls.h>
#include <user/terminal_manager.h>
#include <user/train/train_manager.h>

void TrainTR(char *args);

void TrainRV(char *args);

void TrainTK(char *args);

#endif
