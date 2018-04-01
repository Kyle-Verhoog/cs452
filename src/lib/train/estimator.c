#include <lib/train/estimator.h>

CIRCULAR_BUFFER_DEF(pp_list, pos_event, PREV_POS_LIST_SIZE);

CIRCULAR_BUFFER_DEF(inc_tr_list, train *, INC_TR_LIST_SIZE);

static void train_uninit(train *t) {
  t->num = -1;
  t->curr_pos.pos = NULL;
  t->next_pos.pos = NULL;
  pp_list_init(&t->prev_pos);
  // init speed model
}

static void train_init(train *t, int tnum) {
  t->num = tnum;
  // init speed model with train num
}

int est_last_ts(estimator *est) {
  return est->last_ts;
}

void est_init(estimator *est) {
  int i;

  est->ntrains = 0;
  est->last_ts = 0;

  for (i = 0; i < NUM_TRAINS; ++i) {
    train_uninit(&est->train[i]);
  }

  for (i = 0; i < TRAIN_MAX; ++i) {
    est->tmap[i] = -1;
  }

  for (i = 0; i < SENSOR_SIZE; ++i) {
    inc_tr_list_init(&est->inc_tr[i]);
  }

  for (i = 0; i < SWITCH_SIZE; ++i) {
    est->sw[i].state = '?';
  }
}

// adds a train to the estimator
int est_add_tr(estimator *est, int tr_num, pos_event *pe) {
  train *train;

  if (tr_num < 1 || tr_num > TRAIN_MAX)
    return -1;

  est->tmap[tr_num] = est->ntrains;
  train = &est->train[est->ntrains++];
  train->num = tr_num;
  train->gear = 0;
  train->curr_pos = *pe;

  return 0;
}

pos_event *est_tr_curr_pos(estimator *est, int tr_num) {
  train *train;

  if (est->tmap[tr_num] == -1)
    return NULL;

  train = &est->train[est->tmap[tr_num]];

  return &train->curr_pos;
}

pos_event *est_tr_next_pos(estimator *est, int tr_num) {
  train *train;

  if (est->tmap[tr_num] == -1)
    return NULL;

  train = &est->train[est->tmap[tr_num]];

  return &train->next_pos;
}


// associate a train to a sensor event
static train *assoc_tr(estimator *est, pos_event *pe) {
  int i;
  train *train;
  inc_tr_list *inc_trs;

  assert(pe->pos->type == NODE_SENSOR);


  train = NULL;
  inc_trs = &est->inc_tr[pe->pos->id];

  // check the incoming trains list for the sensor
  if (inc_trs->size > 0) {
    inc_tr_list_get(inc_trs, 0, &train);
    return train;
  }

  // do a first pass check to see if the 
  // train = NULL;
  // for (i = 0; i < est->ntrains; ++i) {
  //   train = &est->train[i];
  //   if (train->next_pos.pos == pe->pos) {
  //     return train;
  //   }
  // }

  return train;
}

// update the estimator with a train at sensor event
int est_update_tr_at(estimator *est, pos_event *pe) {
  train *train;

  // figure out which train this event relates to
  train = assoc_tr(est, pe);
}

// update the estimator with a switch change event
int est_update_sw(estimator *est, int sw_num, int state) {
  est->sw[sw_num].state = state;
  return est_update(est, est->last_ts);
}

int est_update_train(estimator *est, train *train, int ts) {
  int dist;

  dist = 0; // speed model generated dist traveled in time delta
}

// given a timestamp, update the estimate
// returns the number of trains that were updated
// or -1 on error
int est_update(estimator *est, int ts) {
  int i;
  train *train;

  for (i = 0; i < est->ntrains; ++i) {
    train = &est->train[i];
    est_update_train(est, train, ts);
  }

  est->last_ts = ts;
  return 0;
}
