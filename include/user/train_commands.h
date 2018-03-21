#ifndef TRAIN_CMDS_H
#define TRAIN_CMDS_H

#include <defines.h>
#include <lib/parse_args.h>
#include <lib/train/track_data.h>
#include <user/syscalls.h>
#include <user/terminal_manager.h>
#include <user/train/train_provider.h>
#include <user/train/calibration.h>

void TrainTR(char *args);

void TrainRV(char *args);

//void TrainTK(char *args);

void SwitchSW(char *args);

void TrainMS(char *args);




#endif
