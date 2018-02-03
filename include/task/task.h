#ifndef TASK_H
#define TASK_H
#include <defines.h>
#include <types.h>
#include <asm.h>
#include <circularbuffer.h>

// TODO: these are copies of the ones in kernel.h, we should figure out where
//       to put them centrally.

#define MAX_TASK 16

typedef enum TaskStatus { // a task is...
  ACTIVE  = 0,            //  active, currently running
  READY   = 1,            //  ready to be run
  BLOCKED = 2,            //  blocked, waiting for something
  UNINIT  = 3,            //  not yet initialized
  ZOMBIE  = 4,            //  dead... but still alive?
  RCV_BL  = 5,
  SND_BL  = 6,
  RPL_BL  = 7,
} TaskStatus;

//Kernel Handles Task Request
typedef enum TaskRequest {
  TR_PASS           = 0,
  TR_BLOCK          = 1,
  TR_CREATE         = 2,
  TR_MY_TID         = 3,
  TR_MY_PARENT_TID  = 4,
  TR_EXIT           = 5,
  TR_ASSERT         = 6,
  TR_SEND           = 7,
  TR_RECEIVE        = 8,
  TR_REPLY          = 9,
  TR_NS_REG         = 10,
  TR_NS_GET         = 11,
  TR_IRQ            = 12
} TaskRequest;

typedef enum InterruptType{
  IT_SWI = 0,
  IT_IRQ = 1
} InterruptType;

typedef struct TaskDescriptor {
  int  tid; //Task id

  uint32_t  sp;   //stack pointer
  uint32_t  stack_base; //stack base

  uint32_t  psr;    //Status Register

  void* task; //Function pointer
  TaskStatus status;  //Task status
  InterruptType it;  //interrupt type

  struct TaskDescriptor *parent; //Parent task
  struct TaskDescriptor *next;
  int priority;

  CircularBuffer send_q;

  uint32_t ret; //Return value
} TaskDescriptor;

typedef struct TidTracker {
  CircularBuffer cb;
} TidTracker;

void tt_init(TidTracker *tt);
int tt_get(TidTracker *tt);
void tt_return(int tid, TidTracker *tt);
int tt_size(TidTracker *tt);

/**
 * Initialize a task descriptor to be uninitialized.
 */
void td_init(TaskDescriptor *td);

#endif /* TASK_H */
