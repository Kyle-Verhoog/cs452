#include <kernel/syscalls.h>
#include <asm.h>
/**
 * SysCalls (TODO: PASS ENUM LITERAL)
 */

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

tid_t MyTid() {
  //Call swi with myTid
  asm(
    "swi #3;"
  );

  //Expect r0 set
}

tid_t MyParentTid() {
  //Call swi with myTid
  asm(
    "swi #4;"
  );

  //Expect r0 set
}

void Exit() {
  asm("swi #5;");
}

void Assert() {
  asm("swi #6;");
}

void Halt() {
  asm("swi #6;");
}

int Send(tid_t tid, void *msg, int msg_len, void *reply, int reply_len) {
  asm("swi #7;");
  return 0; // TODO
}

int Receive(tid_t *tid, void *msg, int msg_len) {
  asm("swi #8;");
  return 0; // TODO
}

int Reply(tid_t tid, void *reply, int reply_len) {
  asm("swi #9;");
  return 0; // TODO
}

void RegisterNS() {
  asm("swi #10;");
}

int GetNS() {
  asm("swi #11;");
}

int AwaitEvent( int eventid ){
  asm("swi #13;");
}

