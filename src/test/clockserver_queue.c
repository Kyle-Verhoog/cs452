#include <test/clockserver_queue.h>

void csq_init() {
  cs_queue csq;
  csq_init(&csq);
}

void csq_add_1() {
  int ret;
  csq_queue csq;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 10);
  assert(ret == 0);
  assert(csq->size == 1);
}

void csq_check_1() {
  int ret;
  csq_queue csq;
  csq_init(&csq);

  ret = csq_add(&csq, 1, 10);
  assert(ret == 0);
  assert(csq->size == 1);

  ret = csq_check(&csq, 5);
  assert(ret == 0);
  
  ret = csq_check(&csq, 15);
  assert(ret == 1);
}


void clockserver_tests() {
  PTEST_START("clockserver_queue");
  csq_init();
  csq_add_1();
  csq_check_1();
  PTEST_END();
}
