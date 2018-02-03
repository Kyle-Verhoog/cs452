#include <clock_server.h>


void ClockServer(){
	//Wait for a hardware interrupt
	unsigned int origin = 0;

	while(1){
		Pass();
	}
	

	KASSERT(0);
}

void InitClock(){
	//Initialize 32bit clock - Periodic 502KHz
	*(int*)(TIMER3_BASE | LDR_OFFSET) = 5020;
	*(int*)(TIMER3_BASE | CRTL_OFFSET) = ENABLE_MASK | CLKSEL_MASK | MODE_MASK;	


	Create(31, &ClockServer);
	
	while(1){
		(void)ClockServer;
	}
}