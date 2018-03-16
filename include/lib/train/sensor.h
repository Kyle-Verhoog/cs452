#ifndef SENSOR_H
#define SENSOR_H

typedef enum SensorState{
	SEN_OFF = 0,
	SEN_ON = 1
}SensorState;

typedef struct Sensor{
	SensorState state;
	track_node *node;
}Sensor;

#endif //SENSOR_H
