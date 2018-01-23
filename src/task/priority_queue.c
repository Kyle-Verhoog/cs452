#include <priority_queue.h>

void pq_init(priority_queue *pq) {
  pq->size = 0;
  int i;
  for (i = 0; i < NUM_PRIORITIES; i++)
    tq_init(&pq->pqs[i]);
}

int pq_push(priority_queue *pq, int priority, TaskDescriptor *t) {
  tq_push(&pq->pqs[priority], t);
  pq->size++;
  return 0;                        
}

int pq_dumb_pop(priority_queue *pq, TaskDescriptor **t) {
  int i;
  for (i = NUM_PRIORITIES-1; i >= 0; i--) {
    task_queue *tq = &pq->pqs[i];
    if (tq->size > 0) {
      tq_pop(tq, t);
      pq->size--;
      return 0;
    }
  }
  return PQ_ENOTFOUND;
}

/*
int main(void) {
  task_queue tq;
  tq_init(&tq);
  
  TaskDescriptor td; td.tid = 0; td.next = NULL;
  TaskDescriptor td2; td2.tid = 1; td2.next = NULL;
  TaskDescriptor td3; td3.tid = 2; td3.next = NULL;
  
  priority_queue pq;
  pq_init(&pq);
  pq_push(&pq, 0, &td);
  TaskDescriptor *t;
  pq_pop(&pq, &t);
  if (t != &td) printf("WTF\n");
  return 0;
}
*/
