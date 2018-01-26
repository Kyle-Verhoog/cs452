#include <user/test/messaging.h>

int val = 12;

void SimpleSender() {
  bwprintf(COM2, "START1\r\n");
  Pass();
  int msg = val;
  int reply;
  bwprintf(COM2, "START2\r\n");
  Send(1, &msg, sizeof(msg), &reply, sizeof(int));
  assert(reply == val+1);
  bwprintf(COM2, "TEST\r\n");
  Exit();
}

void SimpleReceiver() {
  Pass();
  int tid;
  int msg;
  Receive(&tid, &msg, sizeof(int));
  assert(tid == 1);
  assert(msg == 12);
  int reply = msg + 1;
  Reply(tid, &reply, sizeof(int));
  bwprintf(COM2, "TEST\r\n");
  Exit();
}
