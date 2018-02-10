#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include <task/task_queue.h>

#define NUM_PRIORITIES 32

#define PQ_NONE      0
#define PQ_ENOTFOUND 1
#define PQ_EMPTY     2
#define PQ_PRIORITY  3

typedef struct priority_queue {
  task_queue pqs[NUM_PRIORITIES];
  int size;
  int state;
} priority_queue;


void pq_init(priority_queue *pq);

int pq_push(priority_queue *pq, int priority, TaskDescriptor *t);

int pq_dumb_pop(priority_queue *pq, TaskDescriptor **t);

int pq_pop(priority_queue *pq, TaskDescriptor **t);

#endif /* PRIORITY_QUEUE_H */
