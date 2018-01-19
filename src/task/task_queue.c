#include <include/task/task_queue.h>

void tq_init(task_queue *tq) {
  KASSERT(tq != NULL);
  tq->start = 0;
  tq->size  = 0;
  tq->end   = 0;
  int i;
  for (i = 0; i < TQ_SIZE; i++) tq->q[i] = NULL;
}

int tq_push(task_queue *tq, TaskDescriptor *t) {
  KASSERT(tq != NULL);
  if (tq->size + 1 > TQ_SIZE) {
    return ETQ_FULL;
  }
  tq->q[tq->end] = t;
  tq->size++;
  tq->end = (tq->end + 1) % TQ_SIZE;
  return 0;
}

int tq_pop(task_queue *tq, TaskDescriptor **t) {
  KASSERT((tq != NULL) && (t != NULL));
  if (tq->size <= 0) {
    return ETQ_EMPTY;
  }
  
  *t = tq->q[tq->start];
  tq->q[tq->start] = NULL;
  tq->start = (tq->start + 1) % TQ_SIZE;
  tq->size--;
  return 0;
}

int tq_peek(task_queue *tq, TaskDescriptor **t) {
  KASSERT((tq != NULL) && (t != NULL));
  if (tq->size <= 0) {
    return ETQ_EMPTY;
  }
  *t = &(tq->q[tq->start]);
  return 0;
}

#if DEBUG
void tq_print(task_queue *tq) {
  int end1 = tq->end >= tq->start ? tq->end : TQ_SIZE;
  int i;
  for (i = tq->start; i < end1; i++) {
    bwprintf(LOG_COM, "%x", tq->q[i]);
  }

  int end2 = tq->end >= tq->start ? 0 : tq->end;
  for (i = 0; i < end2; i++) {
    bwprintf(LOG_COM, "%x", tq->q[i]);
  }
  bwprintf(LOG_COM, "\n");
}
#endif
