#ifndef TRAIN_CMDS_H
#define TRAIN_CMDS_H

#include <defines.h>
#include <lib/parse_args.h>
#include <lib/train/track_data.h>
#include <user/syscalls.h>
#include <user/terminal_manager.h>
#include <user/train/train_provider.h>
#include <user/train/calibration.h>
#include <user/train/driver.h>
#include <lib/train/events.h>

void TrainDR(char *args);

void TrainTR(char *args);

void TrainRV(char *args);

void SwitchSW(char *args);

void TrainMS(char *args);

void TrainCAL(char *args);

void TrainTST(char *args);

#endif
