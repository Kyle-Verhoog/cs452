#include <timer_interface.h>

void TimerInterface(){
	Clock clk;
	tid_t my_tid = MyTid();
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	assert(cs_tid >= 0);

	clk_init(&clk);

	int prev_ti, cur_ti;
	prev_ti = cur_ti = Time(cs_tid, my_tid);
	while(true){
		Delay(cs_tid, my_tid, 10);
		int cur_ti = Time(cs_tid, my_tid);
		int diff = cur_ti - prev_ti;

		if(diff >= 10){
			add_dsec(&clk, diff/10);
			//Move the cursor to position
			//Print to Terminal
			//PRINTF("%d:%d:%d\n\r", clk.min, clk.sec, clk.dsec);
			prev_ti += (diff/10)*10;	//Try to be as accurate as possible
		}
	}

	Exit();
}
