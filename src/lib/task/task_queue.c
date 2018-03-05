#include <task/task_queue.h>

void tq_init(task_queue *tq) {
  tq->head = NULL;
  tq->tail = NULL;
  tq->size = 0;
}

int tq_push(task_queue *tq, TaskDescriptor *t) {
  if (tq->size > 0) {
    tq->tail->next = t;
    tq->tail = t;
  } else {
    tq->head = t;
    tq->tail = t;
  }
  tq->size++;

  return 0;
}

int tq_pop(task_queue *tq, TaskDescriptor **t) {
  if (tq->size < 1) return ETQ_EMPTY;
  *t = tq->head;
  tq->head = tq->head->next;
  (*t)->next = NULL;
  tq->size--;

  if (tq->size == 0) {
    tq->head = NULL;
    tq->tail = NULL;
  }
  if (tq->size <= 1) {
    tq->tail = tq->head;
  }

  return 0;
}

int tq_peek(task_queue *tq, TaskDescriptor **t) {
  return 0;
}
