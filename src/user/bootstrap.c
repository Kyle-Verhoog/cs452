#include <user/bootstrap.h>

int stay_alive;

void IdleTask() {
  int i = 0;
  tid_t tx_tid = WhoIs(IOSERVER_UART2_TX_ID);
  assert(tx_tid >= 0);
  while(stay_alive) {
    i++;
    if (i > 15000000) {
      PutC(tx_tid, '?');
      i = 0;
    }
  }
  Exit();
}


void Bootstrap() {
  stay_alive = 1;
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(31, &ClockServer);
  Create(30, &IOServerUART2); // NOTE: priority has to be < priority of IOServer
  Create(30, &IOServerUART1); // NOTE: priority has to be < priority of IOServer
  Create(30, &ReaderServiceUART2);
  // Create(30, &WriterServiceUART2);
  Create(25, &IdleTask);
  // Create(23, &NameServerStop);
  // Create(23, &ClockServerStop);
}
