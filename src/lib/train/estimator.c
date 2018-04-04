#include <lib/train/estimator.h>

CIRCULAR_BUFFER_DEF(pp_list, pos_event, PREV_POS_LIST_SIZE);

CIRCULAR_BUFFER_DEF(inc_tr_list, train *, INC_TR_LIST_SIZE);

EXT_CIRCULAR_BUFFER_DEF(tr_at_list, train *, TR_AT_LIST_SIZE);

CIRCULAR_BUFFER_DEF(tn_list, track_node *, TN_LIST_SIZE);

int tr_at_list_insert(tr_at_list *tr_at, train *tr) {
  int i, r, size;
  train *other;

  size = tr_at->size;
  for (i = 0; i < size; ++i) {
    tr_at_list_pop(tr_at, &other);

    // printf("%d %d\n", tr, other);
    assert(other != tr);

    if (tr && (tr->curr_pos.off > other->curr_pos.off)) {
      tr_at_list_push(tr_at, tr);
      tr = NULL;
    }

    r = tr_at_list_push(tr_at, other);
    assert(r == 0);
  }

  if (tr) {
    tr_at_list_push(tr_at, tr);
  }

  return 0;
}


static void train_uninit(train *t) {
  t->num = -1;
  t->curr_pos.pos = NULL;
  t->curr_pos.ts  = INT_MIN;
  t->next_pos.pos = NULL;
  t->next_pos.ts  = INT_MIN;
  t->next_sen     = NULL;
  pp_list_init(&t->prev_pos);
}

int est_last_ts(estimator *est) {
  return est->last_ts;
}

train *est_get_train(estimator *est, int tr_num) {
  return &est->train[est->tmap[tr_num]];
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
    est->sw[i].state = -1;
    est->sw[i].conf = 0;
    est->sw[i].last_known_state = DIR_CURVED;   // by default set last known to curved
  }
}

// adds a train to the estimator
int est_add_tr(estimator *est, int tr_num, pos_event *pe) {
  train *train;
  tr_at_list *tr_at;

  if (tr_num < 1 || tr_num > TRAIN_MAX)
    return -1;

  est->tmap[tr_num] = est->ntrains;
  train = &est->train[est->ntrains++];
  train->num = tr_num;
  train->gear = 0;
  train->curr_pos = *pe;
  train->len = 200;        // TODO: hard code train length to be 20cm
  train->next_sen = NULL;
  getVelocityModel(&train->s_model, train->num);

  tr_at = &est->tr_at[pe->pos->id];
  tr_at_list_insert(tr_at, train);

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
  // int i;
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

  // TODO if we don't have a train registered it could have taken a wrong path
  //      or been too fast to have registered in time
  assert(0 && "TODO: train finding");

  return train;
}

// finds a train relative to sensor
// looks behind first, and then forward
// returns
//   the dist between the train and the sensor (negative if before, positive if after)
//   INT_MAX if not found
static int est_find_train_rel_to_sensor(estimator *est, train *tr, track_node *sen) {
  int r;
  track_node *tr_pos;

  tr_pos = tr->curr_pos.pos;

  // look backward first

  r = dist_to_node(sen->reverse, tr_pos->reverse);
  if (r >= 0)
    return -r + tr->curr_pos.off;

  // then look forward
  r = dist_to_node(sen, tr_pos);
  if (r >= 0)
    return r - tr->curr_pos.off;

  return INT_MAX;
}



int est_failed_sw(estimator *est, int sw_num, int actual) {
  est->sw[sw_num].conf = 0;
  est->sw[sw_num].last_known_state = actual;
  return 0;
}

int est_estimate_sw_dir(estimator *est, int sw_num) {
  int dir;
  swi *sw;

  sw = &est->sw[sw_num];

  if (sw->conf == 100) {
    dir = sw->state;
  }
  else if (sw->conf == 0) {
    dir = sw->last_known_state;
  }
  else {
    dir = -1;
    assert(0);
  }

  return dir;
}

// returns the next node in the direction the train is facing into `next`
// and the remaining distance to get to it
static int est_next_node(estimator *est, train *train, track_node **next) {
  int dir, offset, dist;
  track_node *curr;

  dist = -1;
  *next = NULL;

  curr   = train->curr_pos.pos;
  offset = train->curr_pos.off;

  switch (curr->type) {
    case NODE_BRANCH:
      dir = est_estimate_sw_dir(est, curr->num);
      assert(dir == DIR_STRAIGHT || dir == DIR_CURVED);
      *next = curr->edge[dir].dest;
      dist = curr->edge[dir].dist - offset;
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


// ------------------------------ TRACK ALGEBRA -------------------------------



// NOTE: this method is not safe in that it does not do any kind of collision
//       detection, usage of this method will allow trains to change order.
//       use with caution
//
// this method mimicks picking up a train and placing it exactly at node `dest`
int est_move_train_to_node_unsafe(estimator *est, train *tr, track_node *dest) {
  int r;
  tr_at_list *tr_at;

  tr_at = &est->tr_at[tr->curr_pos.pos->id];
  r = tr_at_list_rem(tr_at, tr);
  assert(r == 0);

  tr->curr_pos.pos = dest;
  tr->curr_pos.off = 0;

  tr_at = &est->tr_at[tr->curr_pos.pos->id];
  r = tr_at_list_insert(tr_at, tr);
  assert(r == 0);

  return 0;
}

// attempts to move a train forward on the track
//  - if the force arg is asserted, then the train will be moved one train
//    length behind the blocking train
// returns:
//  the remaining distance to move on success
//  -1 if the path was blocked by another train (train position not updated)
//  -2 if the train cannot move because there is a dead end
int est_move_train_forward(estimator *est, train *tr, int dist_to_move, int force, tn_list *past) {
  int i, r, dist_to_next, off_orig;
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
        if (other == tr)
          continue;

        assert(other->curr_pos.pos == tr->curr_pos.pos);

        // check if trains collide
        if (tr->curr_pos.off + dist_to_move > other->curr_pos.off) {
          // move train to right behind the blocking train
          if (force) {
            assert(0);
            r = tr_at_list_rem(tr_at, tr);
            assert(r == 0);
            tr->curr_pos.pos = cur_orig;
            tr->curr_pos.off = off_orig;

            tr_at = &est->tr_at[tr->curr_pos.pos->id];
            r = tr_at_list_insert(tr_at, tr);
            assert(r == 0);
            return dist_to_move;
          }
          else {
            // restore the old state
            tr->curr_pos.pos = cur_orig;
            tr->curr_pos.off = off_orig;
            // have to remove train from tr_at lists we added to
            return -1;
          }
        }
      }
    }

    dist_to_next = est_next_node(est, tr, &next);
    if (dist_to_next < 0) {
      assert(0);
      // restore the old state
      tr->curr_pos.pos = cur_orig;
      tr->curr_pos.off = off_orig;
      return -2;
    }
    assert(next != NULL);

    // printf("togo %d %s %d\n", dist_to_move, tr->curr_pos.pos->name, tr->curr_pos.off);

    // update tr_at of cur and next maintaining ordering
    r = tr_at_list_rem(tr_at, tr);
    assert(r == 0);

    // move to next node
    // printf("COND %d + %d  >= %d\n", tr->curr_pos.off, dist_to_move, dist_to_next);
    if (dist_to_move >= dist_to_next) {
      dist_to_move -= dist_to_next;
      tr->curr_pos.pos = next;
      tr->curr_pos.off = 0;
      tr_at = &est->tr_at[tr->curr_pos.pos->id];
      r = tn_list_push(past, next);
      assert(r == 0);
    }
    else {
      tr->curr_pos.off = tr->curr_pos.off + dist_to_move;
      dist_to_move = 0;
    }

    r = tr_at_list_insert(tr_at, tr);
    assert(r == 0);
  }

  return dist_to_move;
}

int est_move_train_backward(estimator *est, train *train, int dist) {

  return 0;
}


// advances the train taking into consideration the known switch config
// if while progressing this train we run into a train in front, then
// call `est_update_train` on that train to try to get it out of the way
int est_progress_train(estimator *est, train *tr, int dist_to_move, int ts) {
  int i, dist_moved, r;
  train *other;
  track_node *node;
  tr_at_list tr_at;
  tn_list nodes;
  swi *sw;

  tr_at_list_init(&tr_at);
  tn_list_init(&nodes);

  sw = est->sw;

  dist_moved = est_move_train_forward(est, tr, dist_to_move, true, &nodes);

  // train was moved up all the way successfully
  if (dist_moved == 0) {
    for (i = 0; i < nodes.size; ++i) {
      tn_list_get(&nodes, i, &node);

      // add node to the previous positions visited by train
      // r = pp_list_push(&tr->prev_pos, (pos_event){node, 0, ts});
      // assert(r == 0);
      // if (node->type == NODE_SENSOR) {
      //   r = inc_tr_list_push(&est->inc_tr[node->num], tr);
      //   assert(r == 0);
      // }
    }
  }
  // collision with another train, attempt to resolve by updating all trains
  // at the track node and then trying to move again
  else if (dist_moved > 0) {
    tr_at = est->tr_at[tr->curr_pos.pos->id];

    assert(tr_at.size > 0);

    for (i = 0; i < tr_at.size; ++i) {
      tr_at_list_get(&tr_at, i, &other);
      est_update_train(est, other, ts);
    }

    // try moving again after the updates to other trains with remaining dist
    dist_moved += est_move_train_forward(est, tr, dist_to_move-dist_moved, true, &nodes);

    // move was successful
    if (dist_moved == 0) {
      // for (i = 0; i < nodes.size; ++i) {
      //   tn_list_get(&nodes, i, &node);
      //   if (node->type == NODE_SENSOR) {
      //     r = inc_tr_list_push(&est->inc_tr[node->num], tr);
      //     assert(r == 0);
      //   }
      // }
    }
    else if (dist_moved >= 0) {
      // we cannot move up any further, nothing more can be done for this train
      // TODO: maybe update model?
      return 0;
    }
  }
  // dead end
  else if (dist_moved == -2) {
    assert(0 && "dead-end not handled");
  }
  return 0;
}

track_node *get_next_est_sensor_exc(estimator *est, track_node *node) {
  int dir;

  do {
    switch (node->type) {
      case NODE_BRANCH:
        dir = est_estimate_sw_dir(est, node->num);
        break;
      case NODE_EXIT:
        return NULL;
      default:
        dir = DIR_AHEAD;
        break;
    }

    node = node->edge[dir].dest;
  } while (node->type != NODE_SENSOR);

  return node;
}

int est_reg_upcoming_sensor(estimator *est, train *tr) {
  int r;
  track_node *sen;
  inc_tr_list *inc_tr;

  sen = NULL;
  sen = get_next_est_sensor_exc(est, tr->curr_pos.pos);
  if (sen == NULL) {
    return -1;
  }

  // if train isn't registered to this sensor yet, do so
  if (sen != tr->next_sen) {
    inc_tr = &est->inc_tr[sen->num];

    // printf("%s %d\n", sen->name, inc_tr->size);
    r = inc_tr_list_push(inc_tr, tr);
    assert(r == 0);

    tr->next_sen = sen;
  }

  return 0;
}

int est_update_train(estimator *est, train *train, int ts) {
  int r, delta, dist;

  delta = ts - train->curr_pos.ts;

  if (delta > 0) {
    // move train along the track the corresponding distance for time delta
    // dist = 0; // speed model generated dist traveled in time delta
    dist = (delta * interpolate(&train->s_model, train->gear*10))/1000;
    // printf("%d %s %d\n", dist, train->curr_pos.pos->name, train->curr_pos.off);
    r = est_progress_train(est, train, dist, ts);
    if (r) {
    }
  }

  // update the upcoming (estimated) sensor's incoming list
  r = est_reg_upcoming_sensor(est, train);
  if (r) {
    assert(0 && "TODO: deadends");
  }

  train->curr_pos.ts = ts;
  return 0;
}

// given a timestamp, update the estimate
// returns:
//   the number of trains that were updated
//   -1 on error
int est_update(estimator *est, int ts) {
  int i, r;
  train *train;

  // TODO this might not always be the case, but asserting it for now to see
  // when it might happen
  // printf("%d %d\n", ts,est->last_ts);
  assert(ts > est->last_ts);

  if (ts < est->last_ts)
    return 0;

  for (i = 0; i < est->ntrains; ++i) {
    train = &est->train[i];

    // trains should not have a timestamp equal to the new timestamp
    assert(train->curr_pos.ts <= ts);

    // update only trains that have not yet been updated
    if (ts > train->curr_pos.ts) {
      r = est_update_train(est, train, ts);
      if (r) {
        assert(0 && "TODO");
      }
    }
  }

  est->last_ts = ts;
  return 0;
}


// update the estimator with a train at sensor event
int est_update_tr_at(estimator *est, pos_event *pe) {
  int r, rel, ts;
  train *train;

  ts = pe->ts;

  // update everything to current time to get best accuracy in model adjustments
  r = est_update(est, ts);
  if (r) {
    assert(0 && "TODO");
  }

  // figure out which train this event relates to
  train = assoc_tr(est, pe);

  if (train) {
    // TODO

    // 1. figure out where train is relative to sensor
    rel = est_find_train_rel_to_sensor(est, train, pe->pos);


    // 2. update model with knowledge of how far off train is
    if (rel == INT_MAX) {
      assert(0 && "train not found relative to sensor");
    }
    else if (rel < 0) {
      // the train is before the sensor
      printf("BEFORE %d\n", rel);
    }
    else /* if (rel > 0) */ {
      // the train is after the sensor
       printf("AFTER %d\n", rel);
    }

    // 3. move train to sensor
    r = est_move_train_to_node_unsafe(est, train, pe->pos);
    r = est_update_train(est, train, ts);
  }
  else {
  }

  return 0;
}


int est_update_tr_gear(estimator *est, int tr_num, int gear) {
  train *train;

  train = &est->train[est->tmap[tr_num]];
  assert(train->num == tr_num);

  // TODO: initiate some acceleration or deceleration here
  train->gear = gear;

  return 0;
}


// update the estimator with a switch change event
int est_update_sw(estimator *est, int sw_num, int state) {
  est->sw[sw_num].state = state;
  return est_update(est, est->last_ts);
}
