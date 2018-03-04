#include <user/ui/timer_interface.h>

void TimerInterface() {
  Clock clk;
  char buf[25];
  int len, prev_ti, cur_ti, diff;
  tid_t my_tid, cs_tid, tm_tid;

  my_tid = MyTid();
  assert(my_tid >= 0);
  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid >= 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  clk_init(&clk);

  prev_ti = cur_ti = Time(cs_tid, my_tid);
  TMRegister(tm_tid, TIME_OFFSET_X, TIME_OFFSET_Y, TIME_WIDTH, TIME_HEIGHT);

  while (true) {
    Delay(cs_tid, my_tid, 10);
    cur_ti = Time(cs_tid, my_tid);
    diff = cur_ti - prev_ti;

    if (diff >= 10) {
      add_dsec(&clk, diff/10);
      len = buf_pack_f(buf, "\v%t:%t.%d", clk.min, clk.sec, clk.dsec);
      TMPutStr(tm_tid, buf, len);
      prev_ti += (diff/10)*10; // Try to be as accurate as possible
    }
  }

  Exit();
}
