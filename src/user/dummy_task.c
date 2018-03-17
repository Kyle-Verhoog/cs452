#include <user/dummy_task.h>

void DummyTask(char *args) {
  tid_t tm_tid, tid;
  int i, j, k, x, y, w, h, r;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  Receive(&tid, &r, sizeof(r));
  r = parse_args(args, "%d %d %d %d", &x, &y, &w, &h);
  if (r) {
    TMLogStrf(tm_tid, "dummy: error parsing arg %d\n", r);
    r = -1;
    Reply(tid, &r, sizeof(r));
    Exit();
  }

  TMRegister(tm_tid, x, y, w, h);
  r = 0;
  Reply(tid, &r, sizeof(r));
  i = j = k = 0;
  while (true) {
    i++;
    if (i > 2000000) {
      if (j == w-2) {
        TMPutC(tm_tid, '\n');
        if (k == h-2) {
          TMPutC(tm_tid, '\r');
          k = 0;
        } else {
          k++;
        }
        j = 0;
      } else {
        TMPutC(tm_tid, 'a'+j);
        j++;
      }
      i = 0;
    }
  }

  Exit();
}
