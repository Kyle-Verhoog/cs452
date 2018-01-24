#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <system.h>

void Assert();

void Pass();

void Block();

int Create(int priority, void (*code)());

int MyTid();

int MyParentTid();

void Exit();

#define assert(exp) if (KDEBUG)                               \
    do {                                                      \
      if (__predict_false(!(exp))) {                          \
        bwprintf(                                             \
                 LOG_COM,                                     \
                 "\033[31m"                                   \
                 "ASSERTION '"STR(exp)"' FAILED <%s:%d>\r\n"  \
                 "\033[0m",                                   \
                 __FILE__,                                    \
                 __LINE__                                     \
                );                                            \
        Assert();                                             \
      }                                                       \
    } while (0)

#endif /* SYSCALLS_H */
