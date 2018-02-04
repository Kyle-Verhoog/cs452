#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <system.h>

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

void RegisterNS();

int GetNS();

int AwaitEvent( int eventid );

#ifdef DEBUG
#include <io.h>

#define assert(exp)                                    \
  do {                                                 \
    if (__predict_false(!(exp))) {                     \
      PRINTF(                                          \
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
#ifndef X86
#define assert(exp) {}
#endif
#endif /* KDEBUG */
#endif /* SYSCALLS_H */
