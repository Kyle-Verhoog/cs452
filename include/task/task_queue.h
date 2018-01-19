#include <system.h>
#include <task.h>

#define TQ_SIZE 10

// task_queue error codes
#define ETQ_NONE  0
#define ETQ_FULL  1
#define ETQ_EMPTY 2


typedef struct task_queue {
  TaskDescriptor *q[TQ_SIZE];
  int start;
  int end;
  int size;
} task_queue;


void tq_init(task_queue *tq);

/**
 * TODO
 */
int tq_push(task_queue *tq, TaskDescriptor *t);

/**
 * TODO
 */
int tq_pushv(task_queue *tq, uint32_t tid, uint32_t sp, uint32_t psr, void *task, TaskStatus status);

/**
 * TODO
 */
int tq_pop(task_queue *tq, TaskDescriptor **t);

int tq_peek(task_queue *tq, TaskDescriptor **t);

#if DEBUG
#include <bwio.h>
void tq_print(task_queue *tq);
#endif
