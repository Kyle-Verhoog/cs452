#include <user/test/k3_task.h>

#define K3FirstUserTaskID 13
#define K3IdleInterfaceID 14

int exit_flag;

void K3ClientTask() {
  int parent_tid, ii_tid, req, i;
  K3Msg reply;

  // assignment spec indicates that we *immediately* send to the parent task
  // so no nameserver lookup
  // parent_tid = WhoIs(K3FirstUserTaskID);
  // parent_tid = 0;
  parent_tid = MyParentTid();  //Use syscall to obtain parent tid
  ii_tid = WhoIs(K3IdleInterfaceID);

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

  //Tell the idle interface we've finished
  K3IdleRequest ireq = K3_TASK_FINISH;
  K3IdleResponse ires;
  Send(ii_tid, &ireq, sizeof(ireq), &ires, sizeof(ires));

  Exit();
}

void K3IdleInterface(){
  int numClients;
  tid_t requestor;

  //Register
  RegisterAs(K3IdleInterfaceID);

  //Receive number we are waiting for
  Receive(&requestor, &numClients, sizeof(numClients));
  Reply(requestor, &numClients, sizeof(numClients));
  assert(numClients == 4);

  K3IdleRequest ireq;
  K3IdleResponse ires = K3_NOT_FINISHED;

  while(numClients > 0){
    Receive(&requestor, &ireq, sizeof(ireq));
    switch(ireq){
      case K3_TASK_FINISH:
        numClients--;
        Reply(requestor, &ireq, sizeof(ireq));
        break;
      case K3_QUERY_FOR_EXIT:
        Reply(requestor, &ires, sizeof(ires));
        break;
    }
  }

  //We can Exit
  ires = K3_FINISHED;
  Receive(&requestor, &ireq, sizeof(ireq)); //This has to be the idleTask
  Reply(requestor, &ires, sizeof(ires));
  Exit();
}

void K3IdleTask() {
  int mytid = MyTid();
  int cs_tid = WhoIs(CLOCKSERVER_ID);
  int ii_tid = WhoIs(K3IdleInterfaceID);

  K3IdleRequest ireq = K3_QUERY_FOR_EXIT;
  K3IdleResponse ires;

  //Get the starting time and ackowledge ready
  int startTime, endTime, prevTime, curTime;
  unsigned int t3worst, t3best;
  unsigned int t3prev, t3cur, t3session = 0;
  long t3total = 0;
  startTime = prevTime = Time(cs_tid, mytid);
  t3prev = t3cur = 5020;
  t3session = 0;
  t3worst = 5020;
  t3best = 0;

  while (true) {
    curTime = Time(cs_tid, mytid);
    t3cur = *(unsigned int*)(TIMER3_BASE | VAL_OFFSET);

    //Only when interrupt has actually happened
    if(curTime != prevTime){
      t3session += t3prev;  //ran from t3prev to 0 but missed timing
      t3total += t3session;
    
      //Check to quit
      Send(ii_tid, &ireq, sizeof(ireq), &ires, sizeof(ires));
      if(ires == K3_FINISHED){
        break;
      }

      //Update min/max
      if(t3session < t3worst){
        t3worst = t3session;
      }else if(t3session > t3best){
        t3best = t3session;
      }

      //reset
      prevTime = curTime;
      t3session = 0;
    }else{
      t3session += t3prev - t3cur; //Compute delta
    }

    t3prev = t3cur;
  }

  endTime = Time(cs_tid, mytid);
  PRINTF("FINAL METRICS\n\r");
  PRINTF("=============\n\r");
  PRINTF("Idle ran for: \033[33m%d \033[37mof \033[33m%d \033[37mticks\n\r", (t3total/5020), (endTime - startTime));
  PRINTF("Percentage Idle: \033[33m%d%c\033[37m\n\r", (t3total/5020)*100/(endTime - startTime), 37);
  PRINTF("Worst Running Sesson: \033[33m%d.%d \033[37mtick\n\r", ((t3worst*100)/5020)/100, ((t3worst*100)/5020)%100);
  PRINTF("Best Running Sesson: \033[33m%d.%d \033[37mtick\n\r", ((t3best*100)/5020)/100, ((t3best*100)/5020)%100);

  Create(2, &ClockServerStop);
  Create(1, &NameServerStop);

  Exit();
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
  exit_flag = 0;

  // create the name server
  Create(31, &NameServer);
  
  // RegisterAs(K3FirstUserTaskID);

  // create the clock server
  Create(31, &ClockServer);
  
  //IdleTask & Interface
  Create(0, &K3IdleTask);
  tid_t idleInt = Create(1, &K3IdleInterface);

  Create(6, &K3ClientTask);
  Create(5, &K3ClientTask);
  Create(4, &K3ClientTask);
  Create(3, &K3ClientTask);

  //Send to idle interface how many client tasks are running
  int res, numClients = 4;
  Send(idleInt, &numClients, sizeof(numClients), &res, sizeof(res));

  //Kick off user tasks
  tid_t t6, t5, t4, t3;
  Receive(&t6, &req, sizeof(req));
  assert(t6 == 6);
  Receive(&t5, &req, sizeof(req));
  assert(t5 == 7);
  Receive(&t4, &req, sizeof(req));
  assert(t4 == 8);
  Receive(&t3, &req, sizeof(req));
  assert(t3 == 9);
  
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
