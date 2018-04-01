#include <test/train/estimator.h>
#include <lib/train/track_data.h>

static track_node T[TRACK_MAX];

#define POS(p) &T[trhr(T, p)]

static void estimator_init() {
  estimator est;
  est_init(&est);
  assert(sizeof(est) < 256000);
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
  estimator_init();
  estimator_add_train();
  stopped_train_test();
}
