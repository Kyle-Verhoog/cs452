#include <user/test/k3_task.h>

#define K3FirstUserTaskID 13

void K3ClientTask() {
  int parent_tid, req, i;
  K3Msg reply;

  // assignment spec indicates that we *immediately* send to the parent task
  // so no nameserver lookup
  // parent_tid = WhoIs(K3FirstUserTaskID);
  parent_tid = 0;

  Send(parent_tid, &req, sizeof(req), &reply, sizeof(reply));
  assert(reply.n >= 0 && reply.n <= 5000);
  assert(reply.t >= 0 && reply.t <= 1000000);

  int mytid  = MyTid();
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(IS_VALID_TID(mytid));
  assert(IS_VALID_TID(cs_tid));

  for (i = 1; i <= reply.n; i++) {
    Delay(cs_tid, mytid, reply.t);
    PRINTF("tid %x: delay %d, iter %d\r\n", mytid, reply.t, i);
  }

  Exit();
}

void K3IdleTask() {
  int i;
  while (1) {
    for (i = 0; i < 10000000; i++) {
      (void)i;
    }
    PRINTF("ping\r\n");
  }
}

/**
 * Time line of events:
 *
 * - nameserver, clock server initialized
 * - tasks are initialized and blocked on Reply
 *
 * time (ms)       event                                  Output
 *  0         T5 is unblocked and delays for 100ms
 *  0+e       T6 is unblocked and delays for 230ms
 *  0+2e      T7 is unblocked and delays for 330ms
 *  0+3e      T8 is unblocked and delays for 710ms
 *  100       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 1
 *  200       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 2
 *  230       T6 resumes, prints and delays for 230ms   T6: del 23 on iter 1
 *  300       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 3
 *  330       T7 resumes, prints and delays for 330ms   T7: del 33 on iter 1
 *  400       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 4
 *  460       T6 resumes, prints and delays for 230ms   T6: del 23 on iter 2
 *  500       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 5
 *  600       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 6
 *  660       T7 resumes, prints and delays for 330ms   T7: del 33 on iter 2
 *  690       T6 resumes, prints and delays for 230ms   T6: del 23 on iter 3
 *  700       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 7
 *  710       T8 resumes, prints and delays for 710ms   T8: del 71 on iter 1
 *  800       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 8
 *  900       T5 resumes, prints and delays for 100ms   T5: del 10 on iter 9
 *  920       T6 resumes, prints and delays for 230ms   T6: del 23 on iter 4
 *  990       T7 resumes, prints and delays for 330ms   T7: del 33 on iter 3
 *  1000      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 10
 *  1100      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 11
 *  1150      T6 resumes, prints and delays for 230ms   T6: del 23 on iter 5
 *  1200      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 12
 *  1300      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 13
 *  1320      T7 resumes, prints and delays for 330ms   T7: del 33 on iter 4
 *  1380      T6 resumes, prints and delays for 230ms   T6: del 23 on iter 6
 *  1400      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 14
 *  1420      T8 resumes, prints and delays for 710ms   T8: del 71 on iter 2
 *  1500      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 15
 *  1600      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 16
 *  1610      T6 resumes, prints and delays for 230ms   T6: del 23 on iter 7
 *  1650      T7 resumes, prints and delays for 330ms   T7: del 33 on iter 5
 *  1700      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 17
 *  1800      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 18
 *  1840      T6 resumes, prints and delays for 230ms   T6: del 23 on iter 8
 *  1900      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 19
 *  1980      T7 resumes, prints and delays for 330ms   T7: del 33 on iter 6
 *  2000      T5 resumes, prints and delays for 100ms   T5: del 10 on iter 20
 *  2070      T6 resumes, prints and delays for 230ms   T6: del 23 on iter 9
 *  2130      T8 resumes, prints and delays for 710ms   T8: del 71 on iter 3
 */
void K3FirstUserTask() {
  K3Msg reply;
  int req;

  // create the name server
  Create(31, &NameServer);
  
  // RegisterAs(K3FirstUserTaskID);

  // create the clock server
  Create(31, &ClockServer);
  
  Create(0, &K3IdleTask);

  Create(6, &K3ClientTask);
  Create(5, &K3ClientTask);
  Create(4, &K3ClientTask);
  Create(3, &K3ClientTask);

  tid_t t6, t5, t4, t3;
  Receive(&t6, &req, sizeof(req));
  assert(t6 == 5);
  Receive(&t5, &req, sizeof(req));
  assert(t5 == 6);
  Receive(&t4, &req, sizeof(req));
  assert(t4 == 7);
  Receive(&t3, &req, sizeof(req));
  assert(t3 == 8);
  
  reply.n = 20; reply.t = 10;
  Reply(t6, &reply, sizeof(reply));
  reply.n = 9;  reply.t = 23;
  Reply(t5, &reply, sizeof(reply));
  reply.n = 6;  reply.t = 33;
  Reply(t4, &reply, sizeof(reply));
  reply.n = 3;  reply.t = 71;
  Reply(t3, &reply, sizeof(reply));

  Exit();
  // assert(0);
}
