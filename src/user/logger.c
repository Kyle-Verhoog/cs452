#include <user/logger.h>

void Logger() {
  int i, j, r;
  tid_t tm_tid;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  TMRegister(tm_tid, LOG_OFFSET_X, LOG_OFFSET_Y, LOG_WIDTH, LOG_HEIGHT);
  TMLogRegister(tm_tid);

  i = 0;
  j = 0;
  while (true) {
    i++;
    if (i > 20000000) {
      if (j == 18) {
        //TMPutC(tm_tid, '\n');
        j = 0;
      } else {
        TMLogStrf(tm_tid, "test\n");
        // TMPutC(tm_tid, '.');
        j++;
      }
      i = 0;
    }
  }
  Exit();
}
