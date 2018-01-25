#include <k1_task.h>

void taskOne() {
  int tid = MyTid();

  bwprintf(COM2, "T1 - %x: START\r\n", tid);
  Pass();

  bwprintf(COM2, "T1 - %x: MADE NEW TASK: %x\n\r", tid, Create(5, &taskTwo));
  Pass();

  bwprintf(COM2, "T1 - %x: EXIT\n\r", tid);
  // EXIT
  Exit();
}


void taskTwo() {
  int tid = MyTid();
  bwprintf(COM2, "T2 - %x: START\r\n", tid);

  bwprintf(COM2, "T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));
  bwprintf(COM2, "T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));
  bwprintf(COM2, "T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));

  bwprintf(COM2, "T2 - %x: EXIT\r\n", tid);
  Exit();
}


/**
 * K1 Assignment Test Tasks
**/

void OtherTask() {
  int tid = MyTid();
  int parentTid = MyParentTid();

  bwprintf(COM2, "Tid: %x, parentTid: %x\n\r", tid, parentTid);
  Pass();
  bwprintf(COM2, "Tid: %x, parentTid: %x\n\r", tid, parentTid);

  Exit();
}

void FirstUserTask() {
  int val = Create(0, &OtherTask);
  bwprintf(COM2, "Created: %x\n\r", val);
  val = Create(0, &OtherTask);
  bwprintf(COM2, "Created: %x\n\r", val);
  val = Create(6, &OtherTask);
  bwprintf(COM2, "Created: %x\n\r", val);
  val = Create(6, &OtherTask);
  bwprintf(COM2, "Created: %x\n\r", val);

  bwprintf(COM2, "FirstUserTask: exiting.\n\r");
  Exit();
}
