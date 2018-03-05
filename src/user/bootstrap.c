#include <user/bootstrap.h>

void SendGo(){
  tid_t writer = WhoIs(IOSERVER_UART1_TX_ID);
  char command[1];
  command[0] = 96;
  WriteCommandUART1(writer, command, 1);
  Exit();
}

void Bootstrap() {
  int mytid;
  mytid = MyTid();
  Create(31, &NameServer);    // 2
  Create(31, &ClockServer);   // 4
  Create(30, &IOServerUART1); // 9
  Create(30, &IOServerUART2); // 17
  Create(30, &TerminalManager); // 17
  Create(29, &RailwayManager);
  Create(30, &SendGo);
  Create(5, &TimerInterface);
  Create(5, &NProcsInterface);
  Create(5, &MemUsageInterface);
  Create(0, &Logger);
  Exit();
}
