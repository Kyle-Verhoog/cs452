#include <user/test/test_task.h>

#include <asm.h>
#include <ts7200.h>

int c;
void Stall() {
  c += 1;
  int d = c;
  Pass();

  int stallTime = 5000;
  int i;
  for(i = 0; i < stallTime; i+=d) {
    // int reg;
    // int r9, r10, r11, r12;
    // asm("mov r0, %0"::"r"(i*334-321431));
    // asm("mov r1, %0"::"r"(i%2));
    // asm("mov r2, %0"::"r"((i+3)*36));
    // asm("mov r3, %0"::"r"(i-32));
    // asm("mov r4, %0"::"r"(i/10));
    // asm("mov r5, %0"::"r"(i%(i+1)));
    // asm("mov r6, %0"::"r"(i/12));
    // asm("mov r7, %0"::"r"(i+7));
    // asm("mov r8, %0"::"r"(i-212));
    // asm("mov %0, r9":"=r"(r9):);
    // asm("mov %0, r10":"=r"(r10):);
    // asm("mov %0, r11":"=r"(r11):);
    // asm("mov %0, r12":"=r"(r12):);

    Pass();
    // Note r0, r3 corrupted

    /*
    // asm("mov r5, r0");
    asm("mov r6, r1");
    asm("mov r7, r2");
    asm("mov %0, r5":"=r"(reg):);
    assert(reg == (i%2));
    asm("mov %0, r7":"=r"(reg):);
    assert(reg == ((i+3)*36));
    // asm("mov %0, r8":"=r"(reg):);

    assert(0);
    */

    // Note: r1, r2, r3 used by compiler
    // asm("mov %0, r4":"=r"(reg):);
    // assert(reg == i/10 && "r4");
    // asm("mov %0, r5":"=r"(reg):);
    // assert(reg == i%(i+1) && "r5");
    // asm("mov %0, r6":"=r"(reg):);
    // assert(reg == i/12 && "r6");
    // asm("mov %0, r7":"=r"(reg):);
    // assert(reg == i+7 && "r7");
    // asm("mov %0, r8":"=r"(reg):);
    // assert(reg == i-212 && "r8");
    // asm("mov %0, r9":"=r"(reg):);
    // assert(reg == r9);
    // asm("mov %0, r10":"=r"(reg):);
    // assert(reg == r10);
    // asm("mov %0, r11":"=r"(reg):);
    // assert(reg == r11);
    // asm("mov %0, r12":"=r"(reg):);
    // assert(reg == r12);

    //Stalling
    PRINTF("%d: %d \n\r", d, i);
  }

  //Exit();
}

int testRegistersCount;
void TestRegisters() {
  int reg;
  int r9, r10, r11, r12;
  asm("mov r0, #0");
  asm("mov r1, #1");
  asm("mov r2, #2");
  asm("mov r3, #3");
  asm("mov r4, #4");
  asm("mov r5, #5");
  asm("mov r6, #6");
  asm("mov r7, #7");
  asm("mov r8, #8");
  asm("mov %0, r9":"=r"(r9):);
  asm("mov %0, r10":"=r"(r10):);
  asm("mov %0, r11":"=r"(r11):);
  asm("mov %0, r12":"=r"(r12):);
  // asm("mov r9, #9");
  // asm("mov r10, #10");
  // asm("mov r11, #11");
  // asm("mov r12, #12");

  Pass();

  asm("mov %0, r1":"=r"(reg):);
  assert(reg == 1);
  asm("mov %0, r2":"=r"(reg):);
  assert(reg == 2);
  asm("mov %0, r3":"=r"(reg):);
  assert(reg == 3);
  asm("mov %0, r4":"=r"(reg):);
  assert(reg == 4);
  asm("mov %0, r5":"=r"(reg):);
  assert(reg == 5);
  asm("mov %0, r6":"=r"(reg):);
  assert(reg == 6);
  asm("mov %0, r7":"=r"(reg):);
  assert(reg == 7);
  asm("mov %0, r8":"=r"(reg):);
  assert(reg == 8);
  asm("mov %0, r9":"=r"(reg):);
  assert(reg == r9);
  asm("mov %0, r10":"=r"(reg):);
  assert(reg == r10);
  asm("mov %0, r11":"=r"(reg):);
  assert(reg == r11);
  asm("mov %0, r12":"=r"(reg):);
  // assert(reg == r12);

  if(testRegistersCount-- > 0)
    Create(5, &TestRegisters);
  Exit();
}

void TestTid() {
  int tid = MyTid();
  PRINTF("MyTid - %x: \n\r", tid);

  Create(5, &TestTid);
  Exit();
}

int DynamicPriority = 1000;
void DynamicPriorityTest() {

  if(DynamicPriority-- > 0)
    Create(DynamicPriority%32, &DynamicPriorityTest);

  Exit();
}

void GetNameServer() {
  int mytid = MyTid();
  RegisterAs(42);
  int tid = WhoIs(42);
  assert(mytid == tid);
  (void)mytid;
  (void)tid;
  Exit();
}

void IdleTask(){
  PRINTF("Press a key to quit:\n\r");
  bwgetc(COM2);
  ClockServerStop();
  StopNameServer();
  Exit();
}

void TickWaiter(){
  int tid = WhoIs((int)('T'+'C'+'I'));
  PRINTF("Waiting for Clock Interrupt!\n\r");
  int ret = AwaitEvent(IE_TC3UI);
  PRINTF("Clock Interrupt Replied - %d!\n\r", ret);

  Stall();
  
  int finish;
  Send(tid, &tid, sizeof(int), &finish, sizeof(int));

  Stall();

  PRINTF("Waiting for Clock Interrupt!\n\r");
  ret = AwaitEvent(IE_TC3UI);
  PRINTF("Clock Interrupt Replied - %d!\n\r", ret);

  Send(tid, &tid, sizeof(int), &finish, sizeof(int));

  Exit();
}

void TestClockInterrupt(){
  //Initialize 32bit clock - Periodic 502KHz
  *(int*)(TIMER3_BASE | LDR_OFFSET) = 5020;
  *(int*)(TIMER3_BASE | CTRL_OFFSET) = ENABLE_MASK | CLKSEL_MASK | MODE_MASK;

  RegisterAs((int)('T'+'C'+'I'));


  Create(30, &TickWaiter);
  Create(30, &TickWaiter);
  Create(30, &TickWaiter);
  Create(30, &TickWaiter);
  Create(30, &TickWaiter);
  Create(0, &IdleTask);

  int requestor;
  int data;
  int finish = 0;

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  PRINTF("\n\r========================\n\r\n\r");

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  Receive(&requestor, &data, sizeof(int));
  Reply(requestor, &finish, sizeof(int));

  PRINTF("ALL WAITERS HAVE FINISHED WAITING!\n\r");

  //Send to Nameserver to exit
  StopNameServer();
  
  Exit();
}

void CSTestTask() {
  int tid = MyTid();

  while(true) {
    PRINTF("I'm waiting 1\n\r");
    Delay(tid, 100);
    int t = Time(tid);
    PRINTF("I'm waiting 2\n\r");
    DelayUntil(tid, t + 500);
  }
}

void TestTask() {
  c = 0;
  testRegistersCount = 100;
  bwsetfifo(COM2, OFF);
  bwsetfifo(COM1, OFF);
  PRINTF("Starting Test:\n\r");

  //Create the nameserver
  Create(31, &NameServerTask);
  Create(31, &ClockServer);
  Create(31, &ClockServerNotifier);
  Create(5, &CSTestTask);


  Create(0, &IdleTask);
  //Create(10, &TestClockInterrupt);

  //*(int *)(VIC1_BASE + VIC_SOFTINT_OFFSET) = (1 << 30) - 1; 
  // Create(0, &DynamicPriorityTest);
  // Create(1, &FirstUserTask);
  // Create(0, &TestRegisters);
  // Create(0, &SimpleReceiver);
  // Create(0, &SimpleSender);
  // Create(0, &SimpleSender);
  // Create(10, &NameServerTask);
  // Create(5, &GetNameServer);

  PRINTF("All tests completed.\n\r");
  Exit();
}
