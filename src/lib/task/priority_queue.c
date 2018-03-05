#include <task/priority_queue.h>

void pq_init(priority_queue *pq) {
  pq->size  = 0;
  pq->state = 0;
  int i;
  for (i = 0; i < MAX_PRIORITY; i++)
    tq_init(&pq->pqs[i]);
}

int pq_push(priority_queue *pq, int priority, TaskDescriptor *td) {
  if (priority > MAX_PRIORITY || priority < 0) return PQ_PRIORITY;
  tq_push(&pq->pqs[priority], td);
  pq->state |= 1 << priority;
  pq->size++;
  return 0;
}

int pq_dumb_pop(priority_queue *pq, TaskDescriptor **t) {
  if (pq->size < 1) return PQ_EMPTY;
  int i;
  for (i = MAX_PRIORITY-1; i >= 0; i--) {
    task_queue *tq = &pq->pqs[i];
    if (tq->size > 0) {
      tq_pop(tq, t);
      pq->size--;
      return 0;
    }
  }
  return PQ_ENOTFOUND;
}

/**
 *
 */
int pq_pop(priority_queue *pq, TaskDescriptor **t) {
  if (pq->size < 1) return PQ_EMPTY;
  int ret;
  int p;

  p = 31 - __builtin_clz(pq->state);
  task_queue *tq = &pq->pqs[p];

  if ((ret = tq_pop(tq, t)) != 0)
    return ret;

  if (tq->size == 0)
    pq->state ^= 1 << p;

  pq->size--;
  return 0;
}
