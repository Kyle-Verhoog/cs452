#include <ui_test.h>

int ui_alive;
void IdleUITest() {
  int i= 0;
  while (ui_alive) {
    (void)ui_alive;
    i++;
    if (i > 15000) {
      PRINTF("ping\r\n");
      i = 0;
    }
  }
  Exit();
}

void ClearScreen(){
  tid_t writer = WhoIs(WRITERSERVICE_UART2_ID);
  char *command = "\033[2J";
  int size = 6;
  Cursor c;

  c.row = 1;
  c.col = 1;

  WriteCommandUART2(writer, command, size, &c);
  Exit();
}

void UITest() {
  int i, mytid;
  ui_alive = 1;
  mytid = MyTid();
  Create(1, &IdleUITest);
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(30, &IOServerUART2);

  DelayCS(mytid, 100);

  Create(29, &WriterServiceUART2);
  Create(29, &ReaderServiceUART2);

  Create(28, &ClearScreen);
  Create(25, &TimerInterface);
  //Create(25, &WritebackInterface);
  //Create(25, &TrainMsg);
  
  // DelayCS(mytid, 1000);
  // for (i = 0; i < 100000; i++) (void)i;
  // Create(21, &ClockServerStop);
  // stay_alive = 0;
  // Create(20, &NameServerStop);
  COMPLETE_TEST();
}
