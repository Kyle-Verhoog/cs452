#include <user/test/nameserver_test.h>

int registerer_tid;

void RegisterTest() {
  int r;
  int mytid = MyTid();
  registerer_tid = mytid;
  r = RegisterAs(42);
  assert(r == 0);
  int tid = WhoIs(42);
  assert(mytid == tid);
  (void)mytid;
  (void)tid;
  Exit();
}

void WhoIsTest() {
  int tid = WhoIs(42);
  assert(tid == registerer_tid);
  Exit();
}

void InvalidWhoIsTest() {
  int tid = WhoIs(12);
  assert(tid == -1);
  Exit();
}

void DuplicateRegisterTest() {
  int ret;
  int mytid = MyTid();
  registerer_tid = mytid;
  ret = RegisterAs(42);
  assert(ret == 0);
  int tid = WhoIs(42);
  assert(mytid == tid);
  (void)mytid;
  (void)tid;
  Exit();
}


void NameServerTest() {
  // int i;
  registerer_tid = 0;
  // Start and stop nameserver
  Create(31, &NameServer);
  Create(30, &NameServerStop);
 
  // Start and stop again
  Create(29, &NameServer);
  Create(28, &NameServerStop);

  Create(27, &NameServer);
  Create(26, &RegisterTest);
  // for (i = 0; i < 200; i++) Pass();
  Create(25, &WhoIsTest);
  Create(24, &InvalidWhoIsTest);

  // Note: a second register should *just work*
  // Create(24, &DuplicateRegisterTest);
  // Create(23, &WhoIsTest);

  Create(5, &NameServerStop);

  COMPLETE_TEST();
}
