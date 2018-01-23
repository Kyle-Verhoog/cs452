#include <task_queue.h>

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

/*
void tq_print(task_queue *tq) {
  if (tq->size <= 0) {
    printf("\n");
    return;
  }
  TaskDescriptor *td = tq->head;
  do {
    printf("%d ", td->tid);
  } while ((td = td->next) != NULL);
  printf("\n");
}
*/

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
