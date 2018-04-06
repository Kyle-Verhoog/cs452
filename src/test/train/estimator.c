#include <test/train/estimator.h>
#include <lib/train/track_data.h>

static track_node T[TRACK_MAX];
static track_node TB[TRACK_MAX];

#define POS(p) &T[trhr(T, p)]
#define POSB(p) &TB[trhr(TB, p)]

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
  // printf("%d\n", sizeof(est));
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

// stress test an idle state where these is a train sitting stopped on the track
static void estimator_a_ton_of_nothing() {
  const int TICK = 50;
  int i, t, r;
  estimator estimator, *est;
  est = &estimator;
  pos_event pe;
  est_init(est);

  t = 0;

  pe.ts = t+=TICK;
  pe.pos = POS("A1");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  for (i = 0; i < 10000; ++i) {
    r = est_update(est, t+=TICK);
    assert(cur_pos_is(est, 1, POS("A1"), 0));
  }
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
  pos_event pe;
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
  r = est_update_tr_gear(est, 1, 5, t);
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

// stress test a train moving around the track according to just the model
// (no sensor updates or anything else)
static void estimator_a_ton_of_something() {
  const int TICK = 50;
  int i, t, r;
  estimator estimator, *est;
  est = &estimator;
  pos_event pe;
  est_init(est);

  t = 0;

  pe.ts = t+=TICK;
  pe.pos = POS("A1");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  // get the train moving at speed 5
  r = est_update_tr_gear(est, 1, 5, t);
  assert(r == 0);

  for (i = 0; i < 10000; ++i) {
    r = est_update(est, t+=TICK);
    assert(!cur_pos_is(est, 1, POS("A1"), 0));
  }
}

static void estimator_move_train_basic_sensors() {
  const int SPEED = 1400;
  const int TICK = 50;
  const int DIST = (SPEED*TICK)/1000;
  int t, r;
  estimator estimator, *est;
  est = &estimator;
  pos_event pe;
  est_init(est);

  t = 0;

  pe.ts = t+=TICK;
  pe.pos = POS("E7");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("E7"), 0));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E7"), 0));

  // get the train moving at speed 5
  r = est_update_tr_gear(est, 1, 5, t);
  assert(r == 0);

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E7"), DIST));
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E7"), 2*DIST));
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E7"), 3*DIST));
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E7"), 4*DIST));
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E7"), 5*DIST));

  // we should get a sensor event somewhere here if our model was running fast
  // (sensor is 384mm-350mm away)
  pe.ts = t+10;
  pe.pos = POS("D7");
  pe.off = 0;
  r = est_update_tr_at(est, &pe);
  assert(cur_pos_is(est, 1, POS("D7"), 0));

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

  assert(cur_pos_is(est, 1, POS("E10"), 53));

  // train overran E7 before a sensor event, now send sensor event
  pe.ts = t+10;
  pe.pos = POS("E10");
  pe.off = 0;
  r = est_update_tr_at(est, &pe);
  assert(cur_pos_is(est, 1, POS("E10"), 0));
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E10"), ((TICK-10)*SPEED)/1000));

  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("D15"), 21));
}

static void estimator_two_train_collision_1_stopped() {
  const int TICK = 50;
  int t, r;
  estimator est1, *est;
  est = &est1;
  pos_event pe;


  // test train 1 moving at speed 5, train 2 stopped ahead of train 1

  est_init(est);
  t = 0;

  pe.ts = t+=TICK;
  pe.pos = POS("A1");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  pe.ts = t;
  pe.pos = POS("A1");
  pe.off = 200;
  r = est_add_tr(est, 2, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 2, POS("A1"), 200));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 2, POS("A1"), 200));

  r = est_update_tr_gear(est, 1, 5, t);
  assert(r == 0);

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), (1400*TICK)/1000));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 2*(1400*TICK)/1000));

  // train 1 attempted to move up and is immediately behind train 2
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 199));
}

// test train 1 moving at speed 5, train 2 moving at speed 4, they should
// collide eventually
static void estimator_two_train_collision_2_moving() {
  const int TICK = 50;
  int t, r;
  estimator est1, *est;
  est = &est1;
  pos_event pe;

  est_init(est);
  t = 0;

  pe.ts = t+=TICK;
  pe.pos = POS("A1");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("A1"), 0));

  pe.ts = t;
  pe.pos = POS("A1");
  pe.off = 200;
  r = est_add_tr(est, 2, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 2, POS("A1"), 200));

  r = est_update_tr_gear(est, 1, 5, t);
  assert(r == 0);
  r = est_update_tr_gear(est, 2, 3, t);
  assert(r == 0);

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 70));
  assert(cur_pos_is(est, 2, POS("A1"), 230));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 140));
  assert(cur_pos_is(est, 2, POS("MR12"), 29));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("A1"), 210));
  assert(cur_pos_is(est, 2, POS("MR12"), 59));

  // THIS TESTS THE RECURSIVE UPDATE
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR12"), 49));
  assert(cur_pos_is(est, 2, POS("MR12"), 89));

  // COLLISION BETWEEN UPDATES
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR12"), 118));
  assert(cur_pos_is(est, 2, POS("MR12"), 119));

  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("MR12"), 148));
  assert(cur_pos_is(est, 2, POS("MR12"), 149));
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

static void estimator_wrong_path() {
  const int TICK = 50;
  int t, r;
  estimator est1, *est;
  est = &est1;
  pos_event pe;

  est_init(est);
  t = 0;

  pe.ts = t+10;
  pe.pos = POS("D7");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("D7"), 0));

  r = est_update_tr_gear(est, 1, 5, t+10);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("D7"), 0));

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
  assert(cur_pos_is(est, 1, POS("E10"), 333));

  pe.ts = t+10;
  pe.pos = POS("D9");
  pe.off = 0;
  r = est_update_tr_at(est, &pe);
  assert(cur_pos_is(est, 1, POS("D9"), 0));

  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("E12"), 37));
}

static void estimator_wrong_path_multi() {
  const int TICK = 50;
  int t, r;
  estimator est1, *est;
  est = &est1;
  pos_event pe;

  est_init(est);
  t = 0;

  pe.ts = t+10;
  pe.pos = POS("D7");
  pe.off = 0;
  r = est_add_tr(est, 1, &pe);
  assert(r == 0);
  assert(cur_pos_is(est, 1, POS("D7"), 0));

  pe.ts = t+10;
  pe.pos = POS("D7");
  pe.off = 50;
  r = est_add_tr(est, 2, &pe);
  assert(cur_pos_is(est, 2, POS("D7"), 50));

  r = est_update_tr_gear(est, 1, 5, t+10);
  r = est_update_tr_gear(est, 2, 5, t+10);
  assert(cur_pos_is(est, 1, POS("D7"), 0));
  assert(cur_pos_is(est, 2, POS("D7"), 50));

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
  assert(cur_pos_is(est, 1, POS("E10"), 333));

  pe.ts = t+10;
  pe.pos = POS("D9");
  pe.off = 0;
  r = est_update_tr_at(est, &pe);
  assert(cur_pos_is(est, 2, POS("D9"), 0));

  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);

  pe.ts = t+10;
  pe.pos = POS("D9");
  pe.off = 0;
  r = est_update_tr_at(est, &pe);
  assert(cur_pos_is(est, 1, POS("D9"), 0));

  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POS("D9"), 266));
  assert(cur_pos_is(est, 2, POS("E12"), 37));
}

static void estimator_track_b_init() {
  const int TICK = 10;
  int t, r;
  estimator est1, *est;
  est = &est1;
  pos_event pe;

  est_init(est);

  t = 10;
  est_init_trains(est, t, TB, 2);
  assert(cur_pos_is(est, 1, POSB("A1"), 0));
  assert(cur_pos_is(est, 1, POSB("A1"), 0));

  r = est_update(est, t+=TICK);

  r = est_update_tr_gear(est, 1, 14, t+10);
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POSB("A1"), 0));
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POSB("MR12"), 171));

  pe.ts = t+10;
  pe.pos = POSB("C13");
  pe.off = 0;
  r = est_update_tr_at(est, &pe);
  assert(cur_pos_is(est, 1, POSB("C13"), 0));
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  r = est_update(est, t+=TICK);
  assert(cur_pos_is(est, 1, POSB("C13"), 201));
}

void estimator_tests() {
  init_tracka(T);
  init_trackb(TB);
  tr_at_list_insert_test();
  estimator_init();
  estimator_a_ton_of_nothing();
  estimator_add_train();
  estimator_move_train_basic();
  // estimator_a_ton_of_something();
  estimator_move_train_basic_sensors();
  estimator_two_train_collision_1_stopped();
  estimator_two_train_collision_2_moving();
  stopped_train_test();
  estimator_wrong_path();
  estimator_wrong_path_multi();
  // estimator_track_b_init();
}
