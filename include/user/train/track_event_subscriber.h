#ifndef TRACK_EVENT_SUBSCRIBER_H
#define TRACK_EVENT_SUBSCRIBER_H

#include <lib/train/updates.h>

void TrainGearEventSubscriber();
void TrainPositionEventSubscriber()
void TrainSpeedEventSubscriber();
void TrainStatusEventSubscriber();
void SwitchEventSubscriber();
void SensorEventSubscriber();

#endif //TRACK_EVENT_SUBSCRIBER_H;