#ifndef TASK_H
#define TASK_H
#include <defines.h>
#include <types.h>
#include <asm.h>
#include <circularbuffer.h>
#include <ts7200.h>

// TODO: these are copies of the ones in kernel.h, we should figure out where
//       to put them centrally.

#ifdef DEBUG
  #include <io.h>
  #define TASK_METRICS
  #ifdef TASK_METRICS
    #define TM_CLOCK_LDR (TIMER3_BASE | LDR_OFFSET)
    #define TM_CLOCK_VAL (TIMER3_BASE | VAL_OFFSET)
    #define TM_CLOCK_CTRL (TIMER3_BASE | CTRL_OFFSET)
    #define TM_CLOCK_FLAGS (ENABLE_MASK | CLKSEL_MASK)
    #define TM_CLOCK_VALUE 0xffffffff
  #endif //TASK_METRICS
#endif //DEBUG

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
  TR_IRQ            = 12,
  TR_AWAIT_EVENT    = 13,
} TaskRequest;

typedef enum InterruptType{
  IT_SWI = 0,
  IT_IRQ = 1
} InterruptType;

typedef struct TaskDescriptor {
  tid_t  tid; //Task id

  uint32_t  sp;   //stack pointer
  uint32_t  stack_base; //stack base

  uint32_t  psr;    //Status Register

  void* task; //Function pointer
  TaskStatus status;  //Task status
  InterruptType it;  //interrupt type

  struct TaskDescriptor *parent; //Parent task
  struct TaskDescriptor *next;
  int priority;

  tid_t buffer_q[256];    // TODO: CREATE A DEFINE
  CircularBuffer send_q;

  uint32_t ret; //Return value

#ifdef TASK_METRICS
  int start_time;  //Time Task is initialized
  int running_time;//Time Task is active
  int end_time;    //Time Task End
#endif //TASK_METRICS

} TaskDescriptor;

typedef struct TaskSummary{
  int tid;
  int priority;

  int start_time;
  int running_time;
  int end_time;
} TaskSummary;

typedef struct TidTracker {
  tid_t buffer_tid[256];  // TODO: CREATE A DEFINE
  CircularBuffer cb;
} TidTracker;

void tt_init(TidTracker *tt);
tid_t tt_get(TidTracker *tt);
void tt_return(tid_t tid, TidTracker *tt);
int tt_size(TidTracker *tt);

/**
 * Initialize a task descriptor to be uninitialized.
 */
void td_init(TaskDescriptor *td);

#ifdef TASK_METRICS
  void tm_init(); //Task Metrics initialization
  void tm_delta(int st, int et, TaskDescriptor *td);
  void tm_addSummary(TaskDescriptor *td);
  void tm_summarize();
#endif //TASK_METRICS

#endif /* TASK_H */
