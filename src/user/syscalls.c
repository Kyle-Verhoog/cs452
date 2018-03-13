#include <user/syscalls.h>
#include <asm/asm.h>

/**
 * SysCalls (TODO: PASS ENUM LITERAL)
 */
void Pass() {
  asm("swi #0");
}

void Block() {
  asm("swi #1");
}

int Create(int priority, void (*code)()) {
  //r0 = priority, r1 = code
  //Call swi with create
  register uint32_t r0 __asm__("r0");
  asm("swi #2");
  return r0;
}

tid_t MyTid() {
  //Call swi with myTid
  register uint32_t r0 __asm__("r0");
  asm("swi #3");
  return r0;
}

tid_t MyParentTid() {
  //Call swi with myTid
  register uint32_t r0 __asm__("r0");
  asm("swi #4");
  return r0;
}

void Exit() {
  asm("swi #5");
}

void Assert() {
  asm("swi #6");
}

void Halt() {
  asm("swi #15");
}

int Send(tid_t tid, void *msg, int msg_len, void *reply, int reply_len) {
  register uint32_t r0 __asm__("r0");
  asm("swi #7");
  return r0;
}

int Receive(tid_t *tid, void *msg, int msg_len) {
  register uint32_t r0 __asm__("r0");
  asm("swi #8");
  return r0;
}

int Reply(tid_t tid, void *reply, int reply_len) {
  register uint32_t r0 __asm__("r0");
  asm("swi #9");
  return r0;
}

void RegisterNS() {
  asm("swi #10");
}

int GetNS() {
  register uint32_t r0 __asm__("r0");
  asm("swi #11");
  return r0;
}

int AwaitEvent(InterruptEvent eventid){
  register uint32_t r0 __asm__("r0");
  asm("swi #13");
  return r0;
}

int CreateArgs(int priority, void (*code)(), void *args, int argsize) {
  //r0 = priority, r1 = code, r2 = args
  //Call swi with create
  register uint32_t r0 __asm__("r0");
  asm("swi #14");
  return r0;
}

int NProcs() {
  register uint32_t r0 __asm__("r0");
  asm("swi #16");
  return r0;
}

int MemUsage() {
  register uint32_t r0 __asm__("r0");
  asm("swi #17");
  return r0;
}

int CPUUsage(tid_t tid) {
  register uint32_t r0 __asm__("r0");
  asm("swi #18");
  return r0;
}




