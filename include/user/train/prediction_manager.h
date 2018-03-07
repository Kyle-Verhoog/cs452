#ifndef PREDICTION_MANAGER_H
#define PREDICTION_MANAGER_H

#include <ts7200.h>
#include <defines.h>
#include <train_defines.h>
#include <user/syscalls.h>
#include <io.h>
#include <ascii.h>
#include <ioserver.h>
#include <i2a.h>
#include <writerservice.h>
#include <track_data.h>
#include <circular_buffer.h>

#include <train_manager.h>
#include <switch_manager.h>
#include <sensor_manager.h>
#include <user/terminal_manager.h>

#define MAX_STEPS_AHEAD 16

#define INIT_LIVE_TRAINS(live) live.size = 0; \

typedef enum PM_Command{
	PM_SENSOR = 0,
	PM_SWITCH = 1,
	PM_TRAIN = 2,
	PM_PREDICTION = 3,
}PM_Command;

typedef struct LiveTrains{
	TrainDescriptor *buf[TRAIN_SIZE];
	int size;
}LiveTrains;

typedef struct PMProtocol{
	PM_Command pmc;
	void *args;
	int size;
}PMProtocol;

typedef struct OracleArgs{
	track_node *track;
	LiveTrains *live;
}OracleArgs;

typedef struct Destination{
	track_node *node;
	int dist;
}Destination;

void AddTrain(LiveTrains *live, TrainDescriptor *td);

int DistanceBetweenNodes(Switch *sw, track_node *start, track_node *end);
Destination GetPrevSensor(Switch *sw, track_node *n);	//Inclusive, Overshoots distance
Destination GetPrevSensorNI(Switch *sw, track_node *n);	//Exclusive, Undershoots distance

Destination GetNextSensor(Switch *sw, track_node *n);	//Inclusive, Overshoots distance
Destination GetNextSensorNI(Switch *sw, track_node *n);	//Exclusive, Undershoots distance
void PredictionManager();

#endif //PREDICTION_MANAGER_H
