#include <ui_test.h>

int ui_alive;
void IdleUITest() {
  int i= 0;
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

void UITest() {
  int i, mytid;
  ui_alive = 1;
  mytid = MyTid();
  Create(31, &NameServer);
  Create(31, &ClockServer);

  Create(30, &IOServerUART1);
  Create(30, &IOServerUART2);
  Create(30, &WriterServiceUART2);
  Create(30, &SendGo);

  Create(31, &ClearScreen);
  Create(29, &SwitchManager);
  Create(29, &TrainManager);
  //Create(29, &SensorManager);
  Create(29, &RailwayManager);
  Create(30, &ReaderServiceUART2);

  //Create(28, &ClearScreen);
  Create(19, &TimerInterface);
  Create(0, &IdleUITest);
  //Create(25, &TrainMsg);
  // DelayCS(mytid, 1000);
  // for (i = 0; i < 100000; i++) (void)i;
  // Create(21, &ClockServerStop);
  // stay_alive = 0;
  // Create(20, &NameServerStop);
  COMPLETE_TEST();
}
