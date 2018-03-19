#ifndef TRAIN_DEFINES_H
#define TRAIN_DEFINES_H

#define TRAIN_SIZE 81
#define SWITCH_SIZE 157
  #define NORM_SW_LOW 1
  #define NORM_SW_HIGH 18
  #define SPEC_SW_LOW 153
  #define SPEC_SW_HIGH 156
#define DECODER_SIZE 5
#define SENSOR_SIZE DECODER_SIZE*16

typedef enum TR_Direction{
	TR_FORWARD = -1,
	TR_BACKWARD = 1
} TR_Direction;

#endif //TRAIN_DEFINES_H
