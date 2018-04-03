#include <user/init/bootstrap.h>
#include <user/train/driver.h> // here for hack globals

void RailwayInit() {
  DRIVER1_DEF = false;
  DRIVER2_DEF = false;
  tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
  assert(tx_tid >= 0);
  tid_t rx_tid = WhoIs(IOSERVER_UART1_RX_ID);
  assert(rx_tid >= 0);

  //Send Reset Commands to Terminal
  BLPutC(tx_tid, 'X');
  BLPutC(tx_tid, 192);

  //Flush the IO
  FlushIO(tx_tid);
  FlushIO(rx_tid);

  //Send Go
  PutC(tx_tid, 96);

  Create(27, &WaitingRoom);
  Create(27, &Representer);

  Create(21, &ReservationManager);

  Exit();
}

void Bootstrap() {
  int mytid;
  mytid = MyTid();
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(30, &IOServerUART1);
  Create(30, &IOServerUART2);
  Create(30, &TerminalManager);

  Create(30, &TrackDataInit);

  // Create(30, &SendGo);
  Create(30, &RailwayInit);

  // interfaces
   Create(10, &TrainTrackInterface);
   Create(10, &SensorInterface);
   Create(10, &SwitchInterface);
   Create(10, &TrainInterface);
   Create(5, &TimerInterface);
   Create(5, &NProcsInterface);
   Create(5, &MemUsageInterface);
   Create(30, &TaskManagerInterface);

  Create(0, &Logger);

   Create(19, &init_switch);

  // asm(".word 0xe7ffffff");
  // DelayCS(2000);
  // __builtin_trap();

  Exit();
}
