#include <test/train/estimator.h>
#include <lib/train/track_data.h>

static track_node T[TRACK_MAX];

#define POS(p) &T[trhr(T, p)]

static int cur_pos_is(estimator *est, int tr_num, track_node *n, int off) {
  train *tr;
  tr = est_get_train(est, tr_num);
  // printf("test: %s %d\n", n->name, off);
  // printf("actual: %s %d\n", tr->curr_pos.pos->name, tr->curr_pos.off);
  return tr->curr_pos.pos == n && tr->curr_pos.off == off;
}

static void estimator_init() {
  estimator est;
  est_init(&est);
  assert(sizeof(est) < 128000);
  assert(est_last_ts(&est) == 0);
}


static void estimator_add_train() {
  estimator est;
  pos_event pe, *p;
  int r;
  est_init(&est);

  pe.ts = 100;
  pe.pos = POS("A1");
  pe.off = 0;
  r = est_add_tr(&est, 24, &pe);
  assert(r == 0);
  assert(est.train[0].num == 24);
  assert(est.train[0].curr_pos.pos == pe.pos);

  p = est_tr_curr_pos(&est, 24);
  assert(p != NULL);
  assert(p->pos == POS("A1"));
}


static void tr_at_list_insert_test() {
  int r;
  tr_at_list tr_at;
  tr_at_list_init(&tr_at);

  train t1, t2, t3, *t;
  pos_event p;

  p.pos = POS("A1");

  p.off = 100;
  t1.curr_pos = p;
  r = tr_at_list_insert(&tr_at, &t1);
  assert(r == 0);
  assert(tr_at.size == 1);

  p.off = 0;
  t2.curr_pos = p;
  r = tr_at_list_insert(&tr_at, &t2);
  assert(r == 0);
  assert(tr_at.size == 2);

  r = tr_at_list_pop(&tr_at, &t);
  assert(r == 0);
  assert(t == &t1);

  r = tr_at_list_pop(&tr_at, &t);
  assert(r == 0);
  assert(t == &t2);


  p.off = 100;
  t1.curr_pos = p;
  r = tr_at_list_insert(&tr_at, &t1);
  assert(r == 0);
  p.off = 101;
  t2.curr_pos = p;
  r = tr_at_list_insert(&tr_at, &t2);
  assert(r == 0);
  p.off = 102;
  t3.curr_pos = p;
  r = tr_at_list_insert(&tr_at, &t3);
  assert(r == 0);

  r = tr_at_list_get(&tr_at, 0, &t);
  assert(r == 0);
  assert(t == &t3);

  r = tr_at_list_get(&tr_at, 1, &t);
  assert(r == 0);
  assert(t == &t2);

  r = tr_at_list_get(&tr_at, 2, &t);
  assert(r == 0);
  assert(t == &t1);
}


static void estimator_move_train_basic() {
  const int TICK = 50;
  int t, r;
  estimator estimator, *est;
  est = &estimator;
  pos_event pe, *p;
  est_init(est);

  t = 0;

  pe.ts = t+=TICK;
  pe.pos = POS("A1");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  // get the train moving at speed 5
  r = est_update_tr_gear(est, 1, 5);
  assert(r == 0);

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), (1400*TICK)/1000));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 2*(1400*TICK)/1000));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 3*(1400*TICK)/1000));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR12"), 70-21));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR12"), 49+70));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR11"), 1));

  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E10"), 166));
}

static void estimator_move_two_train_basic() {
  const int TICK = 50;
  int t, r;
  estimator estimator, *est;
  est = &estimator;
  pos_event pe, *p;
  est_init(est);

  t = 0;

  pe.ts = t+=TICK;
  pe.pos = POS("A1");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  r = est_update_tr_gear(est, 1, 5);
  assert(r == 0);

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), (1400*TICK)/1000));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 2*(1400*TICK)/1000));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 3*(1400*TICK)/1000));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR12"), 70-21));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR12"), 49+70));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR11"), 1));

}
// test that an added, stopped train has no next prediction
static void stopped_train_test() {
  int r;
  estimator est;
  pos_event pe, *p;
  est_init(&est);

  pe.ts = 0;
  pe.pos = POS("A1");
  pe.off = 0;
  r = est_add_tr(&est, 24, &pe); assert(r == 0);

  p = est_tr_next_pos(&est, 24);
  assert(p->pos == NULL);

  // simulate some timeout-based updates
  r = est_update(&est, 50);
  assert(r >= 0);
}


void estimator_tests() {
  init_tracka(T);
  tr_at_list_insert_test();
  estimator_init();
  estimator_add_train();
  estimator_move_train_basic();
  estimator_move_two_train_basic();
  // stopped_train_test();
}
