#include <clock.h>

void add_dsec(Clock* clk, uint32_t dsecs){
	clk->dsec += dsecs;

	uint32_t overflow = clk->dsec / 10;
	clk->dsec = clk->dsec % 10;

	clk->sec += overflow;
	overflow = clk->sec / 60;
	clk->sec = clk->sec % 60;

	clk->min += overflow;
}

void clk_init(Clock* clk){
	clk->min = 0;
	clk->sec = 0;
	clk->dsec = 0;
}

