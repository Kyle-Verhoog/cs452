#include <user/ui/mem_usage.h>

void MemUsageInterface() {
  tid_t my_tid, tm_tid, cs_tid;
  int mem_usage, len;
  char buf[32];

  my_tid = MyTid();
  assert(my_tid >= 0);
  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid >= 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  TMRegister(tm_tid, MEM_OFF_X, MEM_OFF_Y, MEM_WIDTH, MEM_HEIGHT);

  char *init = "kmem: ";
  TMPutStr(tm_tid, init, strlen(init));

  while (true) {
    mem_usage = MemUsage();
    len = buf_pack_f(buf, "%d%%\b\b", mem_usage, 32);
    TMPutStr(tm_tid, buf, len);
    Delay(cs_tid, my_tid, 100);
  }
  Exit();
}
