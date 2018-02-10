#include <user/test/messaging_test.h>

#define SENDER_ID 3
#define RECVER_ID 4

typedef struct Req {
  char name[20];
  int len;
} Req;

typedef struct Rep {
  char resp[50];
  int meh;
} Rep;

void Sender() {
  tid_t recv_tid;
  int r;
  r = RegisterAs(SENDER_ID);
  assert(r == 0);

  recv_tid = WhoIs(RECVER_ID);
  assert(recv_tid > 0);
  Req req;
  req.name[0] = 'T';
  req.name[1] = 'E';
  req.name[2] = 'S';
  req.name[3] = 'T';
  req.name[4] = 'I';
  req.name[5] = 'N';
  req.name[6] = 'G';
  req.name[7] = '\0';
  req.name[15] = '~';
  req.len = 8;

  Rep rep;
  Send(recv_tid, &req, sizeof(req), &rep, sizeof(rep));
  assert(rep.meh == -5);
  assert(rep.resp[0] == '0');
  assert(rep.resp[1] == '1');
  assert(rep.resp[2] == '2');
  assert(rep.resp[3] == '3');
  assert(rep.resp[4] == '4');
  assert(rep.resp[5] == '5');
  assert(rep.resp[19] == 'z');
  Exit();
}

void Receiver() {
  tid_t send_tid, tid;
  int r;
  r = RegisterAs(RECVER_ID);

  Req req;
  Receive(&tid, &req, sizeof(req));
  send_tid = WhoIs(SENDER_ID);
  assert(send_tid > 0);
  assert(req.len == 8);
  assert(tid == send_tid);
  assert(req.name[0] == 'T');
  assert(req.name[1] == 'E');
  assert(req.name[2] == 'S');
  assert(req.name[3] == 'T');
  assert(req.name[15] == '~');

  Rep rep;
  rep.resp[0] = '0';
  rep.resp[1] = '1';
  rep.resp[2] = '2';
  rep.resp[3] = '3';
  rep.resp[4] = '4';
  rep.resp[5] = '5';
  rep.resp[19] = 'z';
  rep.meh = -5;

  Reply(tid, &rep, sizeof(rep));
  Exit();
}

void MessagingTest() {
  Create(31, &NameServer);
  Create(27, &Receiver);
  Create(27, &Sender);
  Create(25, &NameServerStop);
  COMPLETE_TEST();
}
