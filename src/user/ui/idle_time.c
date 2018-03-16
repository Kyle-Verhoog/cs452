#include <user/ui/idle_time.h>

void IdleTimeInterface() {
  tid_t my_tid, tm_tid, cs_tid, log_tid;
  int usage, len;
  char buf[32];

  my_tid = MyTid();
  assert(my_tid >= 0);
  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid >= 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);
  log_tid = MyParentTid();
  assert(log_tid >= 0);

  TMRegister(tm_tid, IDLE_OFF_X, IDLE_OFF_Y, IDLE_WIDTH, IDLE_HEIGHT);

  while (true) {
    usage = CPUUsage(log_tid);
    len = buf_pack_f(buf, "\vidle: %t%%", usage);
    TMPutStr(tm_tid, buf, len);
    // TMLogStrf(tm_tid, "%d\n", usage);
    Delay(cs_tid, my_tid, 50);
  }
  Exit();
}
