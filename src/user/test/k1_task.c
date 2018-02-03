#include <k1_task.h>

void taskOne() {
  int tid = MyTid();

  PRINTF("T1 - %x: START\r\n", tid);
  Pass();

  PRINTF("T1 - %x: MADE NEW TASK: %x\n\r", tid, Create(5, &taskTwo));
  Pass();

  PRINTF("T1 - %x: EXIT\n\r", tid);
  // EXIT
  Exit();
}


void taskTwo() {
  int tid = MyTid();
  PRINTF("T2 - %x: START\r\n", tid);

  PRINTF("T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));
  PRINTF("T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));
  PRINTF("T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));

  PRINTF("T2 - %x: EXIT\r\n", tid);
  Exit();
}


/**
 * K1 Assignment Test Tasks
**/

void OtherTask() {
  int tid = MyTid();
  int parentTid = MyParentTid();

  PRINTF("Tid: %x, parentTid: %x\n\r", tid, parentTid);
  Pass();
  PRINTF("Tid: %x, parentTid: %x\n\r", tid, parentTid);

  Exit();
}

void FirstUserTask() {
  int val = Create(0, &OtherTask);
  PRINTF("Created: %x\n\r", val);
  val = Create(0, &OtherTask);
  PRINTF("Created: %x\n\r", val);
  val = Create(6, &OtherTask);
  PRINTF("Created: %x\n\r", val);
  val = Create(6, &OtherTask);
  PRINTF("Created: %x\n\r", val);

  PRINTF("FirstUserTask: exiting.\n\r");
  Exit();
}
