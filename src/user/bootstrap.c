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

void Go(){
  tid_t writer = WhoIs(WRITERSERVICE_UART1_ID);
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
  Create(30, &WriterServiceUART1);
  Create(30, &WriterServiceUART2);
  // Create(30, &Go);

  DelayCS(mytid, 100);
  Create(29, &SwitchManager);
  Create(29, &TrainManager);
  Create(29, &SensorManager);
  Create(29, &RailwayManager);
  Create(30, &ReaderServiceUART2);

  //Create(28, &ClearScreen);
  Create(19, &TimerInterface);
  //Create(25, &TrainMsg);
  Create(0, &IdleTask);
}
