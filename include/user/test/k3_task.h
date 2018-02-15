#ifndef K3_TASK_H
#define K3_TASK_H

#include <io.h>
#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/clockserver.h>


typedef struct K3Request {
  int t;   // delay time
  int n;   // number of delays
} K3Msg;

typedef enum K3IdleRequest {
	K3_TASK_FINISH = 0,
	K3_QUERY_FOR_EXIT = 1
} K3IdleRequest;

typedef enum K3IdleResponse {
	K3_NOT_FINISHED = 0,
	K3_FINISHED = 1
} K3IdleResponse;


void K3FirstUserTask();


#endif /* K3_TASK_H */
