#include <user/test/ioserver_test.h>

void IdleTask() {
  int i = 0;
  while(true) {
    i++;
    if (i > 1500000) {
      PRINTF("ping\r\n");
      i = 0;
    }
  }
}


void IOServerTest() {
  Create(31, &IOServerUART2);
  Create(25, &IdleTask);
  COMPLETE_TEST();
}
