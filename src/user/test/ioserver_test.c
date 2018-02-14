#include <user/test/ioserver_test.h>

int stay_alive;
void IdleTask() {
  int i = 0;
  while(stay_alive) {
    i++;
    if (i > 1500000) {
      PRINTF("ping\r\n");
      i = 0;
    }
  }
  Exit();
}

void IOTask() {
  char c;
  tid_t ios_tid;
  ios_tid = WhoIs(IOSERVER_ID);
  assert(ios_tid > 0);

  while (true) {
    c = GetC(ios_tid);
    if (c == 'q') {
      assert(0 && "EXIT");
      stay_alive = 0;
      Exit();
    }
    else {
      PRINTF("%c\r\n", c);
    }
  }
}


void IOServerTest() {
  stay_alive = 1;
  Create(31, &NameServer);
  Create(31, &ClockServer);
  Create(31, &IOServerUART2);
  Create(26, &IOTask);
  Create(25, &IdleTask);
  Create(23, &NameServerStop);
  Create(23, &ClockServerStop);
  COMPLETE_TEST();
}
