#ifndef CLOCK_H
#define CLOCK_H

#include <types.h>

typedef struct Clock{
	uint32_t min;
	uint32_t sec;
	uint32_t dsec;
}Clock;

void add_dsec(Clock* clk, uint32_t dsec);
void clk_init(Clock* clk);

#endif //CLOCK_H
