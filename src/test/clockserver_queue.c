#include <test/clockserver_queue.h>

void csq_init_0() {
  cs_queue csq;
  csq_init(&csq);
  assert(csq.size == 0);
}

void csq_add_1() {
  int ret;
  cs_queue csq;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 10);
  assert(ret == 0);
  assert(csq.size == 1);

  csq_node *csqn = csq_find(&csq, 1);
  assert(csqn->tid == 1);
  assert(csqn->ticks == 10);
  assert(csqn->next == NULL);
}

void csq_add_2() {
  int ret;
  cs_queue csq;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 10);
  assert(ret == 0);
  assert(csq.size == 1);

  ret = csq_add(&csq, 2, 11);
  assert(ret == 0);
  assert(csq.size == 2);

  ret = csq_add(&csq, 3, 12);
  assert(ret == 0);
  assert(csq.size == 3);
}

void csq_add_3() {
  int ret; (void)ret;
  cs_queue csq;
  csq_node *csqn;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 10);
  ret = csq_add(&csq, 2, 15);
  ret = csq_add(&csq, 3, 12);
  ret = csq_add(&csq, 5, 2);
  assert(csq.size == 4);

  // queue should look like:
  // (5, 2), (1, 10), (3, 12), (2, 15)
  // let's confirm this

  // check (5, 2)
  csqn = csq_find(&csq, 5);
  assert(csqn != NULL);
  assert(csqn->tid == 5);
  assert(csqn->ticks == 2);
  assert(csqn->next->tid == 1);

  // check (1, 10)
  csqn = csq_find(&csq, 1);
  assert(csqn != NULL);
  assert(csqn->tid == 1);
  assert(csqn->ticks == 10);
  assert(csqn->next->tid == 3);

  // check (3, 12)
  csqn = csq_find(&csq, 3);
  assert(csqn != NULL);
  assert(csqn->tid == 3);
  assert(csqn->ticks == 12);
  assert(csqn->next->tid == 2);

  // check (2, 15)
  csqn = csq_find(&csq, 2);
  assert(csqn != NULL);
  assert(csqn->tid == 2);
  assert(csqn->ticks == 15);
  assert(csqn->next == NULL);
}

void csq_add_4() {
  int ret; (void)ret;
  cs_queue csq;
  csq_node *csqn;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 10);
  ret = csq_add(&csq, 2, 15);
  ret = csq_add(&csq, 3, 12);
  ret = csq_add(&csq, 5, 2);

  // queue should look like:
  // (5, 2), (1, 10), (3, 12), (2, 15)
  // let's try some additional edge-cases

  // add new item to the start
  // (9, 1), (5, 2), (1, 10), (3, 12), (2, 15)
  ret = csq_add(&csq, 9, 1);
  assert(ret == 0);

  // check (9, 1)
  csqn = csq_find(&csq, 9);
  assert(csqn != NULL);
  assert(csqn->tid == 9);
  assert(csqn->ticks == 1);
  assert(csqn->next->tid == 5);

  // add new item to the end
  // (9, 1), (5, 2), (1, 10), (3, 12), (2, 15), (25, 1241231)
  ret = csq_add(&csq, 14, 1241231);
  assert(ret == 0);

  // check (25, 1241231)
  csqn = csq_find(&csq, 14);
  assert(csqn != NULL);
  assert(csqn->tid == 14);
  assert(csqn->ticks == 1241231);
  assert(csqn->next == NULL);
}

void csq_check_1() {
  int ret;
  cs_queue csq;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 10);
  assert(ret == 0);
  assert(csq.size == 1);

  ret = csq_check(&csq, 5);
  assert(ret == 1);

  ret = csq_check(&csq, 15);
  assert(ret == 0);
}

void csq_check_2() {
  int ret;
  cs_queue csq;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 10);
  assert(ret == 0);
  assert(csq.size == 1);

  ret = csq_check(&csq, 5);
  assert(ret == 1);
  
  ret = csq_check(&csq, 15);
  assert(ret == 0);
}

void csq_pop_1() {
  int ret;
  cs_queue csq;
  csq_init(&csq);
  tid_t tid;

  ret = csq_add(&csq, 3, 10);
  ret = csq_pop(&csq, 5, &tid);
  assert(ret == CSQ_E_READY);
  assert(csq.size == 1);

  ret = csq_pop(&csq, 10, &tid);
  assert(ret == 0);
  assert(tid == 3);
  assert(csq.size == 0);
}

void csq_pop_2() {
  int ret; (void)ret;
  cs_queue csq;
  // csq_node *csqn;
  tid_t tid;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 1);
  ret = csq_add(&csq, 2, 10);
  ret = csq_add(&csq, 3, 10);
  ret = csq_add(&csq, 4, 10);
  ret = csq_add(&csq, 5, 10);
  assert(csq.size == 5);

  // queue should look like:
  // (1, 1), (2, 10), (3, 10), (4, 10), (5, 10)

  // pop off (1, 1)
  ret = csq_pop(&csq, 10, &tid);
  assert(ret == 0);
  assert(tid == 1);
  // (2, 10), (3, 10), (4, 10), (5, 10)

  // pop off (2, 10)
  ret = csq_pop(&csq, 10, &tid);
  assert(ret == 0);
  assert(tid == 2);
  // (3, 10), (4, 10), (5, 10)

  // pop off (3, 10)
  ret = csq_pop(&csq, 10, &tid);
  assert(ret == 0);
  assert(tid == 3);

  // pop off (4, 10)
  ret = csq_pop(&csq, 10, &tid);
  assert(ret == 0);
  assert(tid == 4);
  // (5, 10)

  // pop off (5, 10)
  ret = csq_pop(&csq, 10, &tid);
  assert(ret == 0);
  assert(tid == 5);
  // EMPTY

  // make sure pops when empty fail
  ret = csq_pop(&csq, 10, &tid);
  assert(ret == CSQ_E_EMPTY);
  
  // make sure pops when empty fail
  ret = csq_pop(&csq, 10, &tid);
  assert(ret == CSQ_E_EMPTY);
}

void csq_pop_3() {
  /*
  int ret; (void)ret;
  cs_queue csq;
  csq_node *csqn;
  csq_init(&csq);
  */
}

void clockserver_tests() {
  csq_init_0();
  csq_add_1();
  csq_add_2();
  csq_add_3();
  csq_add_4();
  csq_check_1();
  csq_check_2();
  csq_pop_1();
  csq_pop_2();
  csq_pop_3();
}
