#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <system.h>

#ifdef KDEBUG
#include <bwio.h>
#endif

void Assert();

void Pass();

void Block();

int Create(int priority, void (*code)());

int MyTid();

int MyParentTid();

int Send(int tid, void *msg, int msg_len, void *reply, int reply_len);

int Receive(int *tid, void *msg, int msg_len);

int Reply(int tid, void *reply, int reply_len);

void Halt();

void Exit();

#ifdef KDEBUG
#define assert(exp)                                      \
  do {                                                 \
    if (__predict_false(!(exp))) {                     \
      bwprintf(                                        \
          LOG_COM,                                     \
          "\033[31m"                                   \
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
#define assert(exp) {}
#endif /* KDEBUG */
#endif /* SYSCALLS_H */
