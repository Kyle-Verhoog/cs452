#include <user/bootstrap.h>

int ui_alive;

void IdleTask() {
  tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  assert(tx_tid >= 0);
  while (ui_alive) {
    (void)ui_alive;
  }
  Exit();
}

void ClearScreen(){
  tid_t writer = WhoIs(WRITERSERVICE_UART2_ID);
  Cursor c;
  SET_CURSOR(c, 1, 1);

  WriteStringUART2(writer, "\033[2J", &c);
  Exit();
}

void SendGo(){
  //tid_t writer = WhoIs(WRITERSERVICE_UART1_ID);
  tid_t writer = WhoIs(IOSERVER_UART1_TX_ID);
  char command[1];
  command[0] = 96;
  WriteCommandUART1(writer, command, 1);
  Exit();
}


void Bootstrap() {
  int mytid;
  ui_alive = 1;
  mytid = MyTid();
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(30, &IOServerUART1);
  Create(30, &IOServerUART2);

  Create(30, &WriterServiceUART2);
  Create(30, &SendGo);

  Create(31, &ClearScreen);
  Create(29, &RailwayManager);
  Create(30, &ReaderServiceUART2);

  // Create(19, &TimerInterface);
  Create(0, &IdleTask);

  DelayCS(mytid, 200);
  Create(20, &StoppingServerTest);
  Exit();
}
