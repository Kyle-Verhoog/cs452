#include <user/test/messaging.h>

int val = 12;

void SimpleSender() {
  int msg = val;
  int reply;
  Send(1, &msg, sizeof(msg), &reply, sizeof(int));
  assert(reply == val+1);
  Exit();
}

void SimpleReceiver() {
  Pass();
  Pass();
  Pass();
  int tid;
  int msg;
  Receive(&tid, &msg, sizeof(int));
  assert(tid == 1);
  assert(msg == 12);

  int reply = msg + 1;

  Reply(tid, &reply, sizeof(int));
  Exit();
}
