#include <test/priority_queue.h>

void pq_sanity() {
  priority_queue pq;
  pq_init(&pq);
  assert(pq.size == 0);
}

void pq_push_basic() {
  priority_queue pq;
  pq_init(&pq);

  TaskDescriptor t1;
  int ret;
  ret = pq_push(&pq, 1, &t1);
  assert(ret == 0);
  assert(pq.size == 1);
}

void pq_dpop_basic() {
  priority_queue pq;
  pq_init(&pq);

  int ret;
  TaskDescriptor t1;
  td_init(&t1);
  ret = pq_push(&pq, 1, &t1);

  TaskDescriptor *td;
  ret = pq_dumb_pop(&pq, &td);
  assert(ret == 0);
  assert(&t1 == td);
  assert(pq.size == 0);
}

void pq_dpop_2_same_priorities() {
  priority_queue pq;
  pq_init(&pq);

  int ret;
  TaskDescriptor t1, t2;
  td_init(&t1);
  td_init(&t2);
  ret = pq_push(&pq, 1, &t1);
  ret = pq_push(&pq, 1, &t2);

  TaskDescriptor *td;
  ret = pq_dumb_pop(&pq, &td);
  assert(ret == 0);
  assert(&t1 == td);
  assert(pq.size == 1);

  ret = pq_dumb_pop(&pq, &td);
  assert(ret == 0);
  assert(&t2 == td);
  assert(pq.size == 0);

  ret = pq_dumb_pop(&pq, &td);
  assert(ret == PQ_EMPTY);
}


void pq_dpop_2_different_priorities() {
  priority_queue pq;
  pq_init(&pq);

  int ret;
  TaskDescriptor t1, t2;
  td_init(&t1);
  td_init(&t2);
  ret = pq_push(&pq, 1, &t1);
  ret = pq_push(&pq, 5, &t2);

  TaskDescriptor *td;
  ret = pq_dumb_pop(&pq, &td);
  assert(ret == 0);
  assert(&t2 == td);
  assert(pq.size == 1);

  ret = pq_dumb_pop(&pq, &td);
  assert(ret == 0);
  assert(&t1 == td);
  assert(pq.size == 0);

  ret = pq_dumb_pop(&pq, &td);
  assert(ret == PQ_EMPTY);
}

void pq_dpop_3_different_priorities() {
  priority_queue pq;
  pq_init(&pq);

  int ret;
  TaskDescriptor t1, t2, t3;
  td_init(&t1);
  td_init(&t2);
  td_init(&t3);
  ret = pq_push(&pq, 3, &t2);
  ret = pq_push(&pq, 0, &t3);
  ret = pq_push(&pq, 8, &t1);
  assert(pq.size == 3);

  TaskDescriptor *td;
  ret = pq_dumb_pop(&pq, &td);
  assert(ret == 0);
  assert(&t1 == td);
  assert(pq.size == 2);

  ret = pq_dumb_pop(&pq, &td);
  assert(ret == 0);
  assert(&t2 == td);
  assert(pq.size == 1);

  ret = pq_dumb_pop(&pq, &td);
  assert(ret == 0);
  assert(&t3 == td);
  assert(pq.size == 0);
}


void pq_pop_basic() {
  priority_queue pq;
  pq_init(&pq);

  int ret;
  TaskDescriptor t1;
  td_init(&t1);
  ret = pq_push(&pq, 1, &t1);

  TaskDescriptor *td;
  ret = pq_pop(&pq, &td);
  assert(ret == 0);
  assert(&t1 == td);
  assert(pq.size == 0);
}

void pq_pop_2_same_priorities() {
  priority_queue pq;
  pq_init(&pq);

  int ret;
  TaskDescriptor t1, t2;
  td_init(&t1);
  td_init(&t2);
  ret = pq_push(&pq, 1, &t1);
  ret = pq_push(&pq, 1, &t2);

  TaskDescriptor *td;
  ret = pq_pop(&pq, &td);
  assert(ret == 0);
  assert(&t1 == td);
  assert(pq.size == 1);

  ret = pq_pop(&pq, &td);
  assert(ret == 0);
  assert(&t2 == td);
  assert(pq.size == 0);

  ret = pq_pop(&pq, &td);
  assert(ret == PQ_EMPTY);
}

void pq_pop_3_different_priorities() {
  priority_queue pq;
  pq_init(&pq);

  int ret;
  TaskDescriptor t1, t2, t3;
  td_init(&t1);
  td_init(&t2);
  td_init(&t3);
  ret = pq_push(&pq, 3, &t2);
  ret = pq_push(&pq, 0, &t3);
  ret = pq_push(&pq, 8, &t1);
  assert(pq.size == 3);

  TaskDescriptor *td;
  ret = pq_pop(&pq, &td);
  assert(ret == 0);
  assert(&t1 == td);
  assert(pq.size == 2);

  ret = pq_pop(&pq, &td);
  assert(ret == 0);
  assert(&t2 == td);
  assert(pq.size == 1);

  ret = pq_pop(&pq, &td);
  assert(ret == 0);
  assert(&t3 == td);
  assert(pq.size == 0);
}


static void pq_many_same_priority() {
  priority_queue pq;
  pq_init(&pq);

  int ret;
  TaskDescriptor t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
  td_init(&t1);
  td_init(&t2);
  td_init(&t4);
  td_init(&t5);
  td_init(&t6);
  td_init(&t7);
  td_init(&t8);
  td_init(&t9);
  td_init(&t10);
  ret = pq_push(&pq, 1, &t1);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t2);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t3);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t4);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t5);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t6);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t7);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t8);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t9);
  assert(ret == 0);
  ret = pq_push(&pq, 1, &t10);
  assert(ret == 0);
}

void priority_queue_tests() {
  pq_sanity();
  pq_push_basic();
  pq_dpop_basic();
  pq_dpop_2_different_priorities();
  pq_dpop_2_same_priorities();
  pq_dpop_3_different_priorities();
  pq_pop_basic();
  pq_pop_2_same_priorities();
  pq_pop_3_different_priorities();
  pq_many_same_priority();
}
