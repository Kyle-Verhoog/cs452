#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <types.h>

void Assert();

void Pass();

void Block();

int Create(int priority, void (*code)());

int CreateArgs(int priority, void (*code)(), void *args, int argsize);

tid_t MyTid();

tid_t MyParentTid();

int Send(tid_t tid, void *msg, int msg_len, void *reply, int reply_len);

int Receive(tid_t *tid, void *msg, int msg_len);

int Reply(tid_t tid, void *reply, int reply_len);

void Halt();

void Exit();

void RegisterNS();

int GetNS();

int AwaitEvent(InterruptEvent eventid);

int NProcs();

int MemUsage();

int CPUUsage(tid_t tid);

#ifdef DEBUG
#include <io.h>

#define assert(exp)                                    \
  do {                                                 \
    if (__predict_false(!(exp))) {                     \
*(int *)(VIC1_BASE + VIC_INTENCLEAR_OFFSET) = VIC1_ENABLED; \
*(int *)(VIC2_BASE + VIC_INTENCLEAR_OFFSET) = VIC2_ENABLED; \
      PRINTF(                                          \
          "\033[31m\033[?9l"                           \
          "USER TASK "                                 \
          "ASSERTION '"STR(exp)"' FAILED <%s:%d>\r\n"  \
          "\033[0m",                                   \
          __FILE__,                                    \
          __LINE__                                     \
              );                                       \
      Assert();                                        \
    }                                                  \
  } while (0)
#else
#ifndef X86
#define assert(exp) {}
#endif
#endif /* KDEBUG */
#endif /* SYSCALLS_H */
