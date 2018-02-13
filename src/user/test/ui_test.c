#include <ui_test.h>

int stay_alive;
void IdleUITest() {
  while (stay_alive) {
    (void)stay_alive;
  }
  Exit();
}

void UITest() {
  int i, mytid;
  stay_alive = 1;
  mytid = MyTid();
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(21, &TimerInterface);
  Create(0, &IdleUITest);
  
  DelayCS(mytid, 1000);
  for (i = 0; i < 100000; i++) (void)i;
  Create(21, &ClockServerStop);
  stay_alive = 0;
  Create(20, &NameServerStop);
  COMPLETE_TEST();
}
