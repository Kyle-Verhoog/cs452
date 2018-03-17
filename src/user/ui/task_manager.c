#include <user/ui/task_manager.h>

void TaskManagerInterface() {
  tid_t my_tid, tm_tid, cs_tid, log_tid;
  int len;
  char buf[4096];

  my_tid = MyTid();
  assert(my_tid >= 0);
  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid >= 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);
  log_tid = MyParentTid();
  assert(log_tid >= 0);

  TMRegister(tm_tid, TASK_OFF_X, TASK_OFF_Y, TASK_WIDTH, TASK_HEIGHT);

  while (true) {
    len = TaskInfo(buf);
    TMPutStr(tm_tid, buf, len);
    Delay(cs_tid, my_tid, 20);
  }
  Exit();
}
