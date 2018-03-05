#ifndef WINDOWS_INTERFACE_H
#define WINDOWS_INTERFACE_H

// logging window config
#define LOG_OFFSET_X  72
#define LOG_OFFSET_Y   1
#define LOG_WIDTH     40
#define LOG_HEIGHT    40

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

// sensor window config
#define SENSOR_OFFSET_X  MEM_OFF_X + MEM_WIDTH + 1
#define SENSOR_OFFSET_Y  1
#define SENSOR_WIDTH    31
#define SENSOR_HEIGHT   2
#endif
