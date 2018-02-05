#ifndef K3_TASK_H
#define K3_TASK_H

#include <io/io.h>
#include <kernel/syscalls.h>
#include <user/nameserver.h>
#include <user/clockserver.h>


typedef struct K3Request {
  int t;   // delay time
  int n;   // number of delays
} K3Msg;

void K3FirstUserTask();


#endif /* K3_TASK_H */
