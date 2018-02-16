#include <timer_interface.h>

void TimerInterface(){
	Clock clk;
	tid_t my_tid = MyTid();
	tid_t cs_tid = WhoIs(CLOCKSERVER_ID);
	tid_t ws_tid = WhoIs(WRITERSERVICE_UART2_ID);
  char buf[25];
  int size, offset;
  Cursor cur;
  cur.row = 1;
  cur.col = 25;
  offset = 0;
	assert(cs_tid >= 0);

	clk_init(&clk);

	int prev_ti, cur_ti;
	prev_ti = cur_ti = Time(cs_tid, my_tid);
	while(true){
		Delay(cs_tid, my_tid, 10);
		int cur_ti = Time(cs_tid, my_tid);
		int diff = cur_ti - prev_ti;
    buf[0] = '\033';
    buf[1] = '[';
    buf[2] = 'K';

    if(diff >= 10){
      add_dsec(&clk, diff/10);
      offset = 3;
      i2a(clk.min, &size, buf+offset);
      offset += size;
      buf[offset++] = ':';
      i2a(clk.sec, &size, buf+offset);
      offset += size;
      buf[offset++] = ':';
      i2a(clk.dsec, &size, buf+offset);
      offset += size;
      assert(offset < 20);
      WriteCommandUART2(ws_tid, buf, offset, &cur);
			prev_ti += (diff/10)*10;	//Try to be as accurate as possible
		}
	}

	Exit();
}
