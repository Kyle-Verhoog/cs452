#include <task_queue.h>

#define NUM_PRIORITIES 20


#define PQ_ENOTFOUND 1

typedef struct priority_queue {
  task_queue pqs[NUM_PRIORITIES];
  int size;
} priority_queue;


void pq_init(priority_queue *pq);

int pq_push(priority_queue *pq, int priority, TaskDescriptor *t);

int pq_pop(priority_queue *pq, TaskDescriptor **t);
