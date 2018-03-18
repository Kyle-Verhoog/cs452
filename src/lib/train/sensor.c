#include <lib/train/sensor.h>

void init_sensors(Sensors *sensors){
	for(int i = 0; i < SENSOR_SIZE; i++){
		sensors[i] = SEN_OFF;
	}
}