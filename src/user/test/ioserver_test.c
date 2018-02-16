#include <user/test/ioserver_test.h>

int stay_alive;
void IOIdleTask() {
  int i = 0;
  while(stay_alive) {
    i++;
    if (i > 150000000) {
      PRINTF("ping\r\n");
      i = 0;
    }
  }
  Exit();
}

void IOTask() {
  char c;
  tid_t rios_tid1, tios_tid1, rios_tid2, tios_tid2;
  rios_tid1 = WhoIs(IOSERVER_UART1_RX_ID);
  assert(rios_tid1 > 0);
  tios_tid1 = WhoIs(IOSERVER_UART1_TX_ID);
  assert(tios_tid1 > 0);
  rios_tid2 = WhoIs(IOSERVER_UART2_RX_ID);
  assert(rios_tid2 > 0);
  tios_tid2 = WhoIs(IOSERVER_UART2_TX_ID);
  assert(tios_tid2 > 0);

  while (true) {
    c = GetC(rios_tid2);
    if (c == 'q') {
      assert(0 && "EXIT");
    } else if (c == 'g') {
      PutC(tios_tid1, 96);
    } else if (c == 'z') {
      char test[4];
      test[0] = 't';
      test[1] = 'e';
      test[2] = 's';
      test[3] = 't';
      PutStr(tios_tid2, test, 4);
    } else {
      PutC(tios_tid2, c);
    }
  }
}


void IOServerTest() {
  stay_alive = 1;
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(30, &IOServerUART2); // NOTE: priority has to be < priority of IOServer
  Create(30, &IOServerUART1); // NOTE: priority has to be < priority of IOServer
  // Create(26, &IOTask);
  Create(25, &IOIdleTask);
  Create(23, &NameServerStop);
  Create(23, &ClockServerStop);
  COMPLETE_TEST();
}
