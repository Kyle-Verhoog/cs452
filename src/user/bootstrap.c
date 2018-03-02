#include <user/bootstrap.h>

int ui_alive;

void IdleTask() {
  tid_t tx_tid = WhoIs(IOSERVER_UART2_TX_ID);
  tid_t writer = WhoIs(WRITERSERVICE_UART2_ID);
  assert(tx_tid >= 0);
  int i;
  i = 0;
  Cursor c;
  SET_CURSOR(c, 1, 1);
  while (ui_alive) {
    i++;
    if (i > 200000000) {
      PutC(tx_tid, '.');
      i = 0;
    }
    (void)ui_alive;
  }
  Exit();
}

void ClearScreen(){
  tid_t writer = WhoIs(WRITERSERVICE_UART2_ID);
  Cursor c;
  SET_CURSOR(c, 1, 1);

  WriteStringUART2(writer, "\033[2J", &c);
  assert(writer == 15);

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
  Create(31, &NameServer);  // 2
  Create(31, &ClockServer); // 4
  // Create(30, &IOServerUART1); // 9
  Create(30, &IOServerUART2); // 17

  Create(30, &SendGo);

  Create(31, &ClearScreen);
  Create(29, &SwitchManager);
  Create(29, &TrainManager);
  Create(29, &SensorManager);
  Create(29, &RailwayManager);

  // Create(19, &TimerInterface);
  // Create(0, &IdleTask);

  DelayCS(mytid, 200);
  Create(20, &StoppingServerTest);
  Exit();
}
