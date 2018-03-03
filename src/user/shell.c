#include <user/shell.h>


void Shell() {
  tid_t tm_tid;
  char c;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);

  TMRegister(tm_tid, 2, 28, 68, 15);

  while (true) {
    c = TMGetC(tm_tid);
    TMPutC(tm_tid, c);
  }
}
