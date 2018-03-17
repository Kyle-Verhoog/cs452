#include <user/logger.h>

void Logger() {
  int i, j;
  tid_t tm_tid, tx_tid;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  tx_tid = WhoIs(IOSERVER_UART2_TX_ID);
  assert(tm_tid >= 0);

  TMRegister(tm_tid, LOG_OFF_X, LOG_OFF_Y, LOG_WIDTH, LOG_HEIGHT);
  TMLogRegister(tm_tid);

  Create(5, &IdleTimeInterface);

  i = 0;
  j = 0;
  while (true) {
    i++;
    if (i > 2000000) {
      if (j == 18) {
        //TMPutC(tm_tid, '\n');
        j = 0;
      } else {
        // TMLogStrf(tm_tid, "PING\n");
        // TMPutC(tm_tid, '.');
        j++;
      }
      i = 0;
    }
  }
  Exit();
}
