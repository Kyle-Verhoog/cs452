#include <ui_test.h>

int ui_alive;
void IdleUITest() {
	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
	assert(tx_tid >= 0);
  while (ui_alive) {
    (void)ui_alive;
  }
  Exit();
}

void UITest() {
  int mytid;
  ui_alive = 1;
  mytid = MyTid();
  Create(31, &NameServer);
  Create(31, &ClockServer);

  Create(30, &IOServerUART1);
  Create(30, &IOServerUART2);
  Create(30, &WriterServiceUART2);

  COMPLETE_TEST();
}
