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
  assert(csqn->id == 1);
  assert(csqn->ticks == 10);
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

void csq_pop_1() {
  int ret;
  cs_queue csq;
  csq_init(&csq);
 
  ret = csq_add(&csq, 3, 10);
  assert(ret == 0);
  assert(csq.size == 1);

  tid_id_t id;
  ret = csq_pop(&csq, 5, &id);
  assert(id == 3);
  
  ret = csq_pop(&csq, 5, &id);
  assert(ret == CSQ_NREADY);
}

void clockserver_tests() {
  csq_init_0();
  csq_add_1();
  csq_check_1();
  csq_pop_1();
}
