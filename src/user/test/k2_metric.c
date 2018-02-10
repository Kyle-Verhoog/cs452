#include <k2_metric.h>

#define NUM_SENDS 10000

//Hardcodes:
//Tid 1 = Reciever
//Tid 2 = Sender
//##NO NAMESERVER##

volatile int *rt4_low, *rt4_high;

void K2InitMetricTask() {
  //Setup Timer
  rt4_low = (int*)(TIMER4_LOW);
  rt4_high = (int*)(TIMER4_HIGH);
  *rt4_high = (int)TIMER4_ENABLE_MASK;

  //Create Reciever
#ifdef METRIC_64
  Create(30, &Receiver64);
#elif METRIC_4
  Create(30, &Receiver4);
#endif

  //Create Sender
#ifdef METRIC_64
  Create(31, &Sender64);
#elif METRIC_4
  Create(31, &Sender4);
#endif

  Exit();
}

void Sender4() {
  int req = 0;
  int reply;

  int dbug_ti;
  int dbug_tc;
  int dbug_max = 0;

  int i;
  for(i = 0; i < NUM_SENDS; i++) {
    dbug_ti = *rt4_low;
    Send(1, &req, sizeof(int), &reply, sizeof(int));
    dbug_tc = *rt4_low;
    if(dbug_max < (dbug_tc - dbug_ti)) {
      dbug_max = dbug_tc - dbug_ti;
    }
  }

  PRINTF("Worst Time: %d\n\r", dbug_max);
  Exit();
}

void Receiver4() {
  tid_t requestor;
  int msg;
  int res = 0;

  int i = 0;
  for(i = 0; i < NUM_SENDS; i++) {
    Receive(&requestor, &msg, sizeof(int));
    Reply(requestor, &res, sizeof(int));
  }

  Exit();
}

void Sender64() {
  char req[64] = STRING_64;
  char reply[64];

  int dbug_ti;
  int dbug_tc;
  int dbug_max = 0;

  int i;
  for(i = 0; i < NUM_SENDS; i++) {
    dbug_ti = *rt4_low;
    Send(1, &req, 64, &reply, 64);
    dbug_tc = *rt4_low;
    if(dbug_max < (dbug_tc - dbug_ti)) {
      dbug_max = dbug_tc - dbug_ti;
    }
  }

  PRINTF("Worst Time: %d\n\r", dbug_max);

  Exit();
}

void Receiver64() {
  tid_t requestor;
  char msg[64];
  char res[64] = STRING_64;

  int i = 0;
  for(i = 0; i < NUM_SENDS; i++) {
    Receive(&requestor, &msg, 64);
    Reply(requestor, &res, 64);
  }

  Exit();
}
