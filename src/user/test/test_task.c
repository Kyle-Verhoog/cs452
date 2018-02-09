#include <user/test/test_task.h>

// int c;

// int testRegistersCount;
// void TestRegisters() {
//   int reg;
//   int r9, r10, r11, r12;
//   asm("mov r0, #0");
//   asm("mov r1, #1");
//   asm("mov r2, #2");
//   asm("mov r3, #3");
//   asm("mov r4, #4");
//   asm("mov r5, #5");
//   asm("mov r6, #6");
//   asm("mov r7, #7");
//   asm("mov r8, #8");
//   asm("mov %0, r9":"=r"(r9):);
//   asm("mov %0, r10":"=r"(r10):);
//   asm("mov %0, r11":"=r"(r11):);
//   asm("mov %0, r12":"=r"(r12):);
//   // asm("mov r9, #9");
//   // asm("mov r10, #10");
//   // asm("mov r11, #11");
//   // asm("mov r12, #12");

//   Pass();

//   asm("mov %0, r1":"=r"(reg):);
//   assert(reg == 1);
//   asm("mov %0, r2":"=r"(reg):);
//   assert(reg == 2);
//   asm("mov %0, r3":"=r"(reg):);
//   assert(reg == 3);
//   asm("mov %0, r4":"=r"(reg):);
//   assert(reg == 4);
//   asm("mov %0, r5":"=r"(reg):);
//   assert(reg == 5);
//   asm("mov %0, r6":"=r"(reg):);
//   assert(reg == 6);
//   asm("mov %0, r7":"=r"(reg):);
//   assert(reg == 7);
//   asm("mov %0, r8":"=r"(reg):);
//   assert(reg == 8);
//   asm("mov %0, r9":"=r"(reg):);
//   assert(reg == r9);
//   asm("mov %0, r10":"=r"(reg):);
//   assert(reg == r10);
//   asm("mov %0, r11":"=r"(reg):);
//   assert(reg == r11);
//   asm("mov %0, r12":"=r"(reg):);
//   // assert(reg == r12);

//   if(testRegistersCount-- > 0)
//     Create(5, &TestRegisters);
//   Exit();
// }

// void TestTid() {
//   uint32_t tid = MyTid();
//   PRINTF("MyTid - %x: \n\r", tid);

//   Create(5, &TestTid);
//   Exit();
// }

// int DynamicPriority = 1000;
// void DynamicPriorityTest() {

//   if(DynamicPriority-- > 0)
//     Create(DynamicPriority%32, &DynamicPriorityTest);

//   Exit();
// }

// void IdleTask(){
//   PRINTF("Press a key to quit:\n\r");
//   bwgetc(COM2);
//   ClockServerStop();
//   NameServerStop();
//   Exit();
// }

// void TickWaiter(){
//   uint32_t tid = WhoIs((int)('T'+'C'+'I'));
//   PRINTF("Waiting for Clock Interrupt!\n\r");
//   int ret = AwaitEvent(IE_TC3UI);
//   PRINTF("Clock Interrupt Replied - %d!\n\r", ret);

//   Stall();
  
//   int finish;
//   Send(tid, &tid, sizeof(int), &finish, sizeof(int));

//   Stall();

//   PRINTF("Waiting for Clock Interrupt!\n\r");
//   ret = AwaitEvent(IE_TC3UI);
//   PRINTF("Clock Interrupt Replied - %d!\n\r", ret);

//   Send(tid, &tid, sizeof(int), &finish, sizeof(int));

//   Exit();
// }

// void TestClockInterrupt(){
//   //Initialize 32bit clock - Periodic 502KHz
//   *(int*)(TIMER3_BASE | LDR_OFFSET) = 5020;
//   *(int*)(TIMER3_BASE | CTRL_OFFSET) = ENABLE_MASK | CLKSEL_MASK | MODE_MASK;

//   RegisterAs((int)('T'+'C'+'I'));


//   Create(30, &TickWaiter);
//   Create(30, &TickWaiter);
//   Create(30, &TickWaiter);
//   Create(30, &TickWaiter);
//   Create(30, &TickWaiter);
//   Create(0, &IdleTask);

//   int requestor;
//   int data;
//   int finish = 0;

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   PRINTF("\n\r========================\n\r\n\r");

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   Receive(&requestor, &data, sizeof(int));
//   Reply(requestor, &finish, sizeof(int));

//   PRINTF("ALL WAITERS HAVE FINISHED WAITING!\n\r");

//   //Send to Nameserver to exit
//   NameServerStop();
  
//   Exit();
// }

// void CSTestTask() {
//   uint32_t tid = MyTid();

//   while(true) {
//     PRINTF("I'm waiting 1\n\r");
//     DelayCS(tid, 100);
//     int t = TimeCS(tid);
//     PRINTF("I'm waiting 2\n\r");
//     DelayUntilCS(tid, t + 500);
//   }
// }

void TestTask() {
  tid_t req;
  int ret;
  RUN_TEST_SYNC(req, ret, ContextSwitchTest);
  RUN_TEST_SYNC(req, ret, TaskIdTest);
  RUN_TEST_SYNC(req, ret, TaskIdTest);
  RUN_TEST_SYNC(req, ret, NameServerTest);
  RUN_TEST_SYNC(req, ret, NameServerTest);
  RUN_TEST_SYNC(req, ret, ScheduleTest);
  RUN_TEST_SYNC(req, ret, ScheduleTest);
  RUN_TEST_SYNC(req, ret, ClockServerTest);
  RUN_TEST_SYNC(req, ret, ClockServerTest);
  Exit();
}
