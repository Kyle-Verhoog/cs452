#include <user/test/test_task.h>

void Stall() {
  int stallTime = 10000;
  int i;
  for(i = 0; i < stallTime; i++) {
    //Stalling
    //bwprintf(COM2, "%d \n\r", i);
    asm("mov r0, %0"::"r"(i%1));
    asm("mov r1, %0"::"r"(i%2));
    asm("mov r2, %0"::"r"(i*36));
    asm("mov r3, %0"::"r"(i-32));
    asm("mov r4, %0"::"r"(i/10));
    asm("mov r5, %0"::"r"(i%(i+1)));
    asm("mov r6, %0"::"r"(i/12));
    asm("mov r7, %0"::"r"(i+7));
    asm("mov r8, %0"::"r"(i-212));
    asm("mov r9, %0"::"r"(i+2123));
    asm("mov r10, %0"::"r"(i*77));
    asm("mov r11, %0"::"r"(i/20));
    asm("mov r12, %0"::"r"(i*7100));
    Pass();
  }

  Exit();
}

int testRegistersCount = 100;
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
  // asm("mov %0, r12":"=r"(reg):);
  // assert(reg == r12);

  if(testRegistersCount-- > 0)
    Create(5, &TestRegisters);
  Exit();
}

void TestTid() {
  int tid = MyTid();
  bwprintf(COM2, "MyTid - %x: \n\r", tid);

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

void TestTask() {
  bwprintf(COM2, "Starting Test:\n\r");
  // Create(0, &Stall);
  // Create(0, &DynamicPriorityTest);
  // Create(1, &FirstUserTask);
  // Create(0, &TestRegisters);
  // Create(0, &SimpleReceiver);
  // Create(0, &SimpleSender);
  // Create(0, &SimpleSender);
  Create(10, &NameServerTask);
  Create(5, &GetNameServer);
  bwprintf(COM2, "All tests completed.\n\r");
  Exit();
}
