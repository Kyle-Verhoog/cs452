#include <test/train/priority_queue.h>

static void init() {
  tp_queue tpq;
  tpq_init(&tpq);
  assert(tpq.size == 0);
}

static void add_basic() {
  tp_queue tpq;
  tpq_init(&tpq);

  int val1, val2, val3, val4, val5;
  val1 = 4;
  val2 = 30;
  val3 = 73;
  val4 = 12;
  val5 = 8;

  tpq_add(&tpq, val1, 2);
  assert(tpq.size == 1);
  assert(tpq.tree[0].val == val1);
  assert(tpq.tree[0].priority == 2);
  assert(tpq.map[val1] == 0);

  tpq_add(&tpq, val2, 1);
  assert(tpq.size == 2);
  assert(tpq.tree[0].val == val2);
  assert(tpq.tree[1].val == val1);
  assert(tpq.map[val1] == 1);
  assert(tpq.map[val2] == 0);

  tpq_add(&tpq, val3, 1);
  assert(tpq.size == 3);
  assert(tpq.tree[0].val == val2);
  assert(tpq.tree[1].val == val1);
  assert(tpq.tree[2].val == val3);
  assert(tpq.map[val1] == 1);
  assert(tpq.map[val2] == 0);
  assert(tpq.map[val3] == 2);

  tpq_add(&tpq, val4, 2);
  assert(tpq.tree[0].val == val2);
  assert(tpq.tree[1].val == val1);
  assert(tpq.tree[2].val == val3);
  assert(tpq.tree[3].val == val4);
  assert(tpq.map[val1] == 1);
  assert(tpq.map[val2] == 0);
  assert(tpq.map[val3] == 2);
  assert(tpq.map[val4] == 3);

  tpq_add(&tpq, val5, 0);
  assert(tpq.tree[0].val == val5);
  assert(tpq.tree[0].priority == 0);
  assert(tpq.tree[1].val == val2);
  assert(tpq.tree[1].priority == 1);
  assert(tpq.tree[2].val == val3);
  assert(tpq.tree[2].priority == 1);
  assert(tpq.tree[3].val == val4);
  assert(tpq.tree[3].priority == 2);
  assert(tpq.tree[4].val == val1);
  assert(tpq.tree[4].priority == 2);
  assert(tpq.map[val1] == 4);
  assert(tpq.map[val2] == 1);
  assert(tpq.map[val3] == 2);
  assert(tpq.map[val4] == 3);
  assert(tpq.map[val5] == 0);
}

static void pop_basic() {
  tp_queue tpq;
  tpq_init(&tpq);

  int val1, val2, val3, val4, val5;
  val1 = 4;
  val2 = 30;
  val3 = 73;
  val4 = 12;
  val5 = 8;
  tpq_add(&tpq, val1, 2);
  tpq_add(&tpq, val2, 1);
  tpq_add(&tpq, val3, 1);
  tpq_add(&tpq, val4, 2);
  tpq_add(&tpq, val5, 0);

  assert(val5 == tpq_pop(&tpq));
  assert(tpq.tree[0].val == val2);
  assert(tpq.tree[1].val == val1);
  assert(tpq.tree[2].val == val3);
  assert(tpq.tree[3].val == val4);
  assert(tpq.map[val1] == 1);
  assert(tpq.map[val2] == 0);
  assert(tpq.map[val3] == 2);
  assert(tpq.map[val4] == 3);
  assert(val2 == tpq_pop(&tpq));
  assert(tpq.tree[0].val == val3);
  assert(tpq.tree[1].val == val1);
  assert(tpq.tree[2].val == val4);
  assert(tpq.map[val1] == 1);
  assert(tpq.map[val3] == 0);
  assert(tpq.map[val4] == 2);
  assert(val3 == tpq_pop(&tpq));
  assert(tpq.tree[0].val == val4);
  assert(tpq.tree[1].val == val1);
  assert(val4 == tpq_pop(&tpq));
  assert(tpq.tree[0].val == val1);
  assert(val1 == tpq_pop(&tpq));
}

static void dec_key_basic() {
  tp_queue tpq;
  tpq_init(&tpq);

  int val1, val2, val3, val4, val5;
  val1 = 4;
  val2 = 30;
  val3 = 73;
  val4 = 12;
  val5 = 8;
  tpq_add(&tpq, val1, 2);
  tpq_add(&tpq, val2, 1);
  tpq_add(&tpq, val3, 1);
  tpq_add(&tpq, val4, 2);
  tpq_add(&tpq, val5, 5);

  tpq_dec_priority(&tpq, val5, 0);
  assert(tpq.tree[0].val == val5);
  assert(tpq.tree[0].priority == 0);
  assert(tpq.tree[1].val == val2);
  assert(tpq.tree[1].priority == 1);
  assert(tpq.tree[2].val == val3);
  assert(tpq.tree[2].priority == 1);
  assert(tpq.tree[3].val == val4);
  assert(tpq.tree[3].priority == 2);
  assert(tpq.tree[4].val == val1);
  assert(tpq.tree[4].priority == 2);
  assert(tpq.map[val1] == 4);
  assert(tpq.map[val2] == 1);
  assert(tpq.map[val3] == 2);
  assert(tpq.map[val4] == 3);
  assert(tpq.map[val5] == 0);

  assert(val5 == tpq_pop(&tpq));

  tpq_dec_priority(&tpq, val4, 0);
  assert(tpq.tree[0].val == val4);
  assert(tpq.tree[0].priority == 0);
  assert(tpq.tree[1].val == val2);
  assert(tpq.tree[1].priority == 1);
  assert(tpq.tree[2].val == val3);
  assert(tpq.tree[2].priority == 1);
  assert(tpq.tree[3].val == val1);
  assert(tpq.tree[3].priority == 2);
  assert(tpq.map[val4] == 0);
  assert(tpq.map[val2] == 1);
  assert(tpq.map[val3] == 2);
  assert(tpq.map[val1] == 3);
}

void train_priority_queue_tests() {
  init();
  add_basic();
  pop_basic();
  dec_key_basic();
}
