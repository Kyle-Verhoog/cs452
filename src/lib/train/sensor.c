#include <lib/train/sensor.h>

void init_sensors(Sensors *sensors){
	int i;
	for(i = 0; i < DECODER_SIZE * 2; i++){
		sensors->sensors[i] = 0;
	}
}
