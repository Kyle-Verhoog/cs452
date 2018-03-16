#ifndef WINDOWS_INTERFACE_H
#define WINDOWS_INTERFACE_H

// logging window config
#define LOG_OFFSET_X  72
#define LOG_OFFSET_Y   1
#define LOG_WIDTH     40
#define LOG_HEIGHT    50

#define CAL_OFFSET_X  72
#define CAL_OFFSET_Y  43
#define CAL_WIDTH     40
#define CAL_HEIGHT    2


// timer window config
#define TIME_OFFSET_X  2
#define TIME_OFFSET_Y  1
#define TIME_WIDTH     10
#define TIME_HEIGHT    2

#define NPROCS_OFF_X   TIME_OFFSET_X + TIME_WIDTH + 1
#define NPROCS_OFF_Y   1
#define NPROCS_WIDTH   15
#define NPROCS_HEIGHT  2

#define MEM_OFF_X NPROCS_OFF_X + NPROCS_WIDTH + 1
#define MEM_OFF_Y      1
#define MEM_WIDTH      10
#define MEM_HEIGHT     2

// shell window config
#define SH_OFFSET_X  2
#define SH_OFFSET_Y  4
#define SH_WIDTH    69
#define SH_HEIGHT    9

// idle
#define IDLE_OFF_X  MEM_OFF_X + MEM_WIDTH + 1
#define IDLE_OFF_Y  1
#define IDLE_WIDTH    11
#define IDLE_HEIGHT   2

// sensor window config
#define SENSOR_OFFSET_X  IDLE_OFF_X + IDLE_WIDTH + 1
#define SENSOR_OFFSET_Y  1
#define SENSOR_WIDTH    19
#define SENSOR_HEIGHT   2

// sensor window config
#define M1_RESET_OFF_X   SH_OFFSET_X
#define M1_RESET_OFF_Y   SH_OFFSET_Y + SH_HEIGHT + 1
#define M1_RESET_WIDTH   SH_WIDTH
#define M1_RESET_HEIGHT  10

#define TRACK_OFF_X SH_OFFSET_X
#define TRACK_OFF_Y SH_OFFSET_Y + SH_HEIGHT + 1
#define TRACK_WIDTH SH_WIDTH
#define TRACK_HEIGHT 40

#endif
