#include <lib/train/estimator.h>

CIRCULAR_BUFFER_DEF(pp_list, pos_event, PREV_POS_LIST_SIZE);

CIRCULAR_BUFFER_DEF(inc_tr_list, train *, INC_TR_LIST_SIZE);

EXT_CIRCULAR_BUFFER_DEF(tr_at_list, train *, TR_AT_LIST_SIZE);

static void train_uninit(train *t) {
  t->num = -1;
  t->curr_pos.pos = NULL;
  t->curr_pos.ts = INT_MIN;
  t->next_pos.pos = NULL;
  t->next_pos.ts = INT_MIN;
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

  for (i = 0; i < TRACK_MAX; ++i) {
    tr_at_list_init(&est->tr_at[i]);
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
  train->len = 200;        // TODO: hard code train length to be 20cm

  assert(train->curr_pos.ts >= 0);
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

  return train;
}

// update the estimator with a train at sensor event
int est_update_tr_at(estimator *est, pos_event *pe) {
  train *train;

  // figure out which train this event relates to
  train = assoc_tr(est, pe);
}


int est_update_tr_gear(estimator *est, int tr_num, int gear) {

  return 0;
}

// update the estimator with a switch change event
int est_update_sw(estimator *est, int sw_num, int state) {
  est->sw[sw_num].state = state;
  return est_update(est, est->last_ts);
}

// returns the next node in the direction the train is facing into `next`
// and the remaining distance to get to it
static int est_next_node(estimator *est, train *train, track_node **next) {
  int offset, dist;
  track_node *curr;

  dist = -1;
  *next = NULL;

  curr   = train->curr_pos.pos;
  offset = train->curr_pos.off;

  switch (curr->type) {
    case NODE_BRANCH:
      if (est->sw[curr->num].state == SW_STRAIGHT) {
        *next = curr->edge[DIR_STRAIGHT].dest;
        dist = curr->edge[DIR_STRAIGHT].dist - offset;
      }
      else if (est->sw[curr->num].state == SW_CURVE) {
        *next = curr->edge[DIR_CURVED].dest;
        dist = curr->edge[DIR_CURVED].dist - offset;
      }
      else {
        assert(0);
      }
      break;
    case NODE_EXIT:
      break;
    default:
      *next = curr->edge[DIR_AHEAD].dest;
      dist = curr->edge[DIR_AHEAD].dist - offset;
      break;
  }

  return dist;
}


int est_update_train(estimator *est, train *train, int ts);


static int tr_at_list_insert(tr_at_list *tr_at, train *tr) {
  int i, r, size;
  train *other;
  tr_at_list temp;

  tr_at_list_init(&temp);

  size = tr_at->size;
  for (i = 0; i < size; ++i) {
    tr_at_list_pop(tr_at, &other);
    if (other == tr) return -1;

    if (tr && (tr->curr_pos.off > other->curr_pos.off || i == size-1)) {
      tr_at_list_push(tr_at, &tr);
      tr = NULL;
    }
    r = tr_at_list_push(tr_at, &other);
    if (r) return r;
  }

  return 0;
}


// ------------------------------ TRACK ALGEBRA -------------------------------

// attempts to move a train forward on the track
//  - if the force arg is asserted, then the train will be moved one train
//    length behind the blocking train
// returns:
//  0 if the move was successful (updating the train position and tr_at lists)
//  1 if the path was blocked by another train (train position not updated)
//  2 if the train cannot move because there is a dead end
int est_move_train_forward(estimator *est, train *tr, int dist_to_move, int force) {
  int i, r, dist_moved, dist_to_next, off_orig;
  track_node *cur_orig, *next;
  train *other;
  tr_at_list *tr_at;

  off_orig = tr->curr_pos.off;
  cur_orig = tr->curr_pos.pos;
  tr_at = &est->tr_at[tr->curr_pos.pos->id];

  while (dist_to_move > 0) {
    if (tr_at->size > 0) {
      // iterate through the tr_at list descending (closest trains first)
      // there are other trains on this track node, determine if we can move
      for (i = tr_at->size-1; i >= 0; --i) {
        tr_at_list_get(tr_at, i, &other);

        assert(other->curr_pos.pos == tr->curr_pos.pos);

        // check if trains collide
        if (tr->curr_pos.off + dist_to_move > other->curr_pos.off) {
          // move train to right behind the blocking train
          if (force) {
          }
          else {
            // restore the old state
            tr->curr_pos.pos = cur_orig;
            tr->curr_pos.off = off_orig;
            return 1;
          }
        }
      }
    }

    dist_to_next = est_next_node(est, tr, &next);
    if (dist_to_next < 0) {
      // restore the old state
      tr->curr_pos.pos = cur_orig;
      tr->curr_pos.off = off_orig;
      return 2;
    }
    if (tr->curr_pos.off + dist_to_move >= dist_to_next) {
      // update tr_at of cur and next maintaining ordering
      r = tr_at_list_rem(tr_at, tr);
      assert(r == 0);


      dist_to_move -= dist_to_next - tr->curr_pos.off;

      tr->curr_pos.pos = next;
      tr->curr_pos.off = 0;
      tr_at = &est->tr_at[tr->curr_pos.pos->id];
      r = tr_at_list_insert(tr_at, tr);
      assert(r == 0);
    }
    else {
      tr->curr_pos.off = tr->curr_pos.off + dist_to_move;
      dist_to_move = 0;
    }
  }


  return 0;
}

int est_move_train_backward(estimator *est, train *train, int dist) {

}



// advances the train taking into consideration the known switch config
// if while progressing this train we run into a train in front, then
// call `est_update_train` on that train to try to get it out of the way
int est_progress_train(estimator *est, train *tr, int dist_to_move, int ts) {
  int i, dist_moved, dist_to_next, ntrains, r;
  train *other;
  track_node *next;
  tr_at_list *tr_at;
  swi *sw;

  sw = est->sw;

  r = est_move_train_forward(est, tr, dist_to_move, true);

  // collision with another train, attempt to resolve by updating all trains
  // at the track node and then trying to move again
  if (r == 1) {

  }
  // dead end
  else if (r == 2) {
  }
}

int est_update_train(estimator *est, train *train, int ts) {
  int r, delta, dist;
  track_node *sensor;
  inc_tr_list *inc_tr;

  delta = ts - train->curr_pos.ts;
  assert(delta > 0);

  // move train along the track the corresponding distance for time delta
  dist = 0; // speed model generated dist traveled in time delta
  r = est_progress_train(est, train, dist, ts);

  // update the upcoming (estimated) sensor's incoming list
  sensor = NULL; // get_next_sensor_exc();
  // inc_tr = est->inc_tr[sensor->id];
  train->curr_pos.ts = ts;
}

// given a timestamp, update the estimate
// returns the number of trains that were updated
// or -1 on error
int est_update(estimator *est, int ts) {
  int i;
  train *train;

  if (ts < est->last_ts)
    return 0;

  for (i = 0; i < est->ntrains; ++i) {
    train = &est->train[i];

    // trains should not have a timestamp equal to the new timestamp
    assert(train->curr_pos.ts != ts);

    // update only trains that have not yet been updated
    if (ts > train->curr_pos.ts) {
      est_update_train(est, train, ts);
    }
  }

  est->last_ts = ts;
  return 0;
}
