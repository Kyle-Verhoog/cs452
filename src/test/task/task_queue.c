#include <test/task_queue.h>

void tq_sanity() {
  task_queue tq;
  tq_init(&tq);
  assert(tq.size == 0);
}

void tq_push_basic() {
  task_queue tq;
  tq_init(&tq);
  assert(tq.size == 0);
  TaskDescriptor t;
  tq_push(&tq, &t);
  assert(tq.size == 1);
  tq_push(&tq, &t);
  tq_push(&tq, &t);
  assert(tq.size == 3);
}

/**
 * Pushes 1 task to queue and validates popped result.
 */
void tq_pop_basic() {
  task_queue tq;
  tq_init(&tq);
  TaskDescriptor t1, t2;
  td_init(&t1);
  td_init(&t2);

  tq_push(&tq, &t1);
  assert(tq.size == 1);

  TaskDescriptor *td;
  int ret;

  ret = tq_pop(&tq, &td);
  assert(tq.size == 0);
  assert(ret == 0);
  assert(td == &t1);
  ret = tq_pop(&tq, &td);
  assert(ret == ETQ_EMPTY);
}

/**
 * Pushes 2 tasks to queue and validates popped results.
 */
void tq_pop_2() {
  task_queue tq;
  tq_init(&tq);
  TaskDescriptor t1, t2;
  td_init(&t1);
  td_init(&t2);

  TaskDescriptor *td;
  int ret;

  tq_push(&tq, &t1);
  assert(tq.size == 1);
  tq_push(&tq, &t2);
  assert(tq.size == 2);

  ret = tq_pop(&tq, &td);
  assert(tq.size == 1);
  assert(ret == 0);
  assert(td == &t1);
  ret = tq_pop(&tq, &td);
  assert(tq.size == 0);
  assert(ret == 0);
  assert(td == &t2);
}

/**
 * Pushes 2 tasks to queue and validates popped results twice
 */
void tq_pop_22() {
  task_queue tq;
  tq_init(&tq);
  TaskDescriptor t1, t2;
  td_init(&t1);
  td_init(&t2);

  TaskDescriptor *td;
  int ret;

  tq_push(&tq, &t1);
  assert(tq.size == 1);
  tq_push(&tq, &t2);
  assert(tq.size == 2);

  ret = tq_pop(&tq, &td);
  assert(tq.size == 1);
  assert(ret == 0);
  assert(td == &t1);
  ret = tq_pop(&tq, &td);
  assert(tq.size == 0);
  assert(ret == 0);
  assert(td == &t2);

  tq_push(&tq, &t1);
  assert(tq.size == 1);
  tq_push(&tq, &t2);
  assert(tq.size == 2);

  ret = tq_pop(&tq, &td);
  assert(tq.size == 1);
  assert(ret == 0);
  assert(td == &t1);
  ret = tq_pop(&tq, &td);
  assert(tq.size == 0);
  assert(ret == 0);
  assert(td == &t2);
}

void task_queue_tests() {
  tq_sanity();
  tq_push_basic();
  tq_pop_basic();
  tq_pop_2();
  tq_pop_22();
  printf("✓ task_queue\n");
}
