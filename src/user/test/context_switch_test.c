#include <user/test/context_switch_test.h>

int c;

void Stall() {
  c += 1;
  int d = c;
  Pass();

  int stallTime = 5000;
  int i;
  for(i = 0; i < stallTime; i+=d) {
    int reg;
    int r9, r10, r11, r12;
    asm("mov r0, %0"::"r"(i*334-321431));
    asm("mov r1, %0"::"r"(i%2));
    asm("mov r2, %0"::"r"((i+3)*36));
    asm("mov r3, %0"::"r"(i-32));
    asm("mov r4, %0"::"r"(i/10));
    asm("mov r5, %0"::"r"(i%(i+1)));
    asm("mov r6, %0"::"r"(i/12));
    asm("mov r7, %0"::"r"(i+7));
    asm("mov r8, %0"::"r"(i-212));
    asm("mov %0, r9":"=r"(r9):);
    asm("mov %0, r10":"=r"(r10):);
    asm("mov %0, r11":"=r"(r11):);
    asm("mov %0, r12":"=r"(r12):);

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
    // //assert(reg == i/12 && "r6");
    // asm("mov %0, r7":"=r"(reg):);
    // assert(reg == i+7 && "r7");
    // asm("mov %0, r8":"=r"(reg):);
    // assert(reg == i-212 && "r8");
    asm("mov %0, r9":"=r"(reg):);
    assert(reg == r9);
    asm("mov %0, r10":"=r"(reg):);
    assert(reg == r10);
    asm("mov %0, r11":"=r"(reg):);
    assert(reg == r11);
    // asm("mov %0, r12":"=r"(reg):);
    // assert(reg == r12);

    //Stalling
    // PRINTF("%d: %d \n\r", d, i);
  }

  Exit();
}


void ContextSwitchTest() {
  c = 0;
  Create(30, &Stall);

  COMPLETE_TEST();
}
