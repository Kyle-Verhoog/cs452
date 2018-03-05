#include <user/ui/num_procs.h>

void NProcsInterface() {
  tid_t my_tid, tm_tid, cs_tid;
  int nprocs, len;
  char buf[32];

  my_tid = MyTid();
  assert(my_tid >= 0);
  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid >= 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  TMRegister(tm_tid, NPROCS_OFF_X, NPROCS_OFF_Y, NPROCS_WIDTH, NPROCS_HEIGHT);

  char *init = "#tasks: ";
  TMPutStr(tm_tid, init, strlen(init));

  while (true) {
    nprocs = NProcs();
    len = buf_pack_f(buf, "%t/%d\b\b\b\b\b", nprocs, MAX_TASK);
    TMPutStr(tm_tid, buf, len);
    Delay(cs_tid, my_tid, 100);
  }
  Exit();
}
