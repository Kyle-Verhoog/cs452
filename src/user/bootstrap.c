#include <user/bootstrap.h>

int ui_alive;

void IdleTask2() {
  int i;

  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  TMRegister(tm_tid);

  i = 0;
  while (ui_alive) {
    i++;
    if (i > 20000000) {
      TMPutC(tm_tid, ';');
      // PutC(tx_tid, '.');
      i = 0;
    }
  }
  Exit();
}

void IdleTask() {
  int i;

  tid_t tx_tid = WhoIs(IOSERVER_UART2_TX_ID);
  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tx_tid >= 0);
  assert(tm_tid >= 0);

  TMRegister(tm_tid);

  i = 0;
  while (ui_alive) {
    i++;
    if (i > 20000000) {
      TMPutC(tm_tid, '.');
      // PutC(tx_tid, '.');
      i = 0;
    }
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
  Create(31, &NameServer);    // 2
  Create(31, &ClockServer);   // 4
  Create(30, &IOServerUART1); // 9
  Create(30, &IOServerUART2); // 17

  Create(30, &TerminalManager); // 17

  // Create(30, &WriterServiceUART2);
  // Create(30, &SendGo);

  // Create(31, &ClearScreen);
  // Create(29, &SwitchManager);
  // Create(29, &TrainManager);
  // Create(29, &SensorManager);
  // Create(29, &RailwayManager);
  //Create(30, &ReaderServiceUART2);

  Create(0, &IdleTask);
  Create(0, &IdleTask2);
  Exit();
}
