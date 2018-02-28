#include <user/test/clockserver_test.h>

volatile int stay_alive;
void IdleTest() {
  while (stay_alive) {
    (void)stay_alive;
  }
  Exit();
}

void TimeTest() {
  volatile int ti, tf, tid, i;
  tid = MyTid();
  ti = TimeCS(tid);
  for (i = 0; i < 10000; i++)
    (void)i;
  tf = TimeCS(tid);
  assert(tf >= ti);
  Exit();
}

void ClockServerStopper(){
  volatile int mytid;
  mytid = MyTid();
  DelayCS(mytid, 300);
  Create(13, &ClockServerStop);
  Create(12, &NameServerStop);
  stay_alive = 0;
  Exit();
}

void ClockServerTest() {
  volatile int mytid;
  stay_alive = 1;
  mytid = MyTid();
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(25, &TimeTest);
  Create(25, &TimeTest);
  Create(1, &ClockServerStopper);
  Create(1, &IdleTest);
  COMPLETE_TEST();
}

