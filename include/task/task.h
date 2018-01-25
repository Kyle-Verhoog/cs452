#ifndef TASK_H
#define TASK_H
#include <defines.h>
#include <types.h>
#include <asm.h>
#include <circularbuffer.h>

// TODO: these are copies of the ones in kernel.h, we should figure out where
//       to put them centrally.

#define USER_MODE 16
#define KERNEL_MODE 19
#define SYSTEM_MODE 31
#define USER_STACK_BASE 0x02000000
#define USER_STACK_SIZE 0x100000  //1 MB User stacks

#define MAX_TASK 16

typedef enum TaskStatus { // a task is...
  ACTIVE  = 0,           //  active, currently running
  READY   = 1,           //  ready to be run
  BLOCKED = 2,           //  blocked, waiting for something
  UNINIT  = 3,           //  not yet initialized
  ZOMBIE  = 4,           //  dead... but still alive?
} TaskStatus;

//Kernel Handles Task Request
typedef enum TaskRequest {
  PASS = 0,
  BLOCK = 1,
  CREATE = 2,
  MY_TID = 3,
  MY_PARENT_TID = 4,
  EXIT = 5,
  ASSERT = 6,
} TaskRequest;

typedef volatile struct TaskDescriptor {
  int  tid; //Task id

  uint32_t  sp;   //stack pointer
  uint32_t  stack_base; //stack base

  uint32_t  psr;    //Status Register

  void* task; //Function pointer
  TaskStatus status;  //Task status

  volatile struct TaskDescriptor *parent; //Parent task
  volatile struct TaskDescriptor *next;
  int priority;

  uint32_t ret; //Return value
} TaskDescriptor;

typedef struct TidTracker {
  CircularBuffer cb;
} TidTracker;

void tt_init(volatile TidTracker *tt);
int tt_get(volatile TidTracker *tt);
void tt_return(int tid, volatile TidTracker *tt);

/**
 * Initialize a task descriptor to be uninitialized.
 */
void td_init(volatile TaskDescriptor *td);

#endif /* TASK_H */
