#include <kernel/syscalls.h>

/**
 * SysCalls (TODO: PASS ENUM LITERAL)
 */

void Assert() {
  asm (
    "swi #6;"
  );
}

void Pass() {
  asm (
    "swi #0;"
  );
}

void Block() {
  asm (
    "swi #1;"
  );
}

int Create(int priority, void (*code)()) {
  //r0 = priority, r1 = code
  //Call swi with create
  asm(
    "swi #2;"
  );

  //Expect from here r0 is set already as the return value
}

int MyTid() {
  //Call swi with myTid
  asm(
    "swi #3;"
  );

  //Expect r0 set
}

int MyParentTid() {
  //Call swi with myTid
  asm(
    "swi #4;"
  );

  //Expect r0 set
}

void Exit() {
  asm (
    "swi #5;"
  );
}
