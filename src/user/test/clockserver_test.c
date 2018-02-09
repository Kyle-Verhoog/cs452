#include <user/test/clockserver_test.h>

int stay_alive;
void IdleTest() {
  while (stay_alive) {
    (void)stay_alive;
  }
  Exit();
}

void TimeTest() {
  int ti, tf, tid, i;
  tid = MyTid();
  ti = TimeCS(tid);
  for (i = 0; i < 10000; i++)
    (void)i;
  tf = TimeCS(tid);
  assert(tf > ti);
  Exit();
}

void ClockServerTest() {
  int i, mytid;
  stay_alive = 1;
  mytid = MyTid();
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(25, &TimeTest);
  Create(25, &TimeTest);
  Create(0, &IdleTest);
  DelayCS(mytid, 100);
  stay_alive = 0;
  Create(21, &ClockServerStop);
  for (i = 0; i < 10000; i++) (void)i;
  Create(20, &NameServerStop);
  COMPLETE_TEST();
}

