#include <lib/train/estimator.h>

// CIRCULAR_BUFFER_DEF(pp_list, pos_event, PREV_POS_LIST_SIZE);

EXT_CIRCULAR_BUFFER_DEF(sen_reg_list, train *, INC_TR_LIST_SIZE);

EXT_CIRCULAR_BUFFER_DEF(tr_at_list, train *, TR_AT_LIST_SIZE);

CIRCULAR_BUFFER_DEF(tn_list, track_node *, TN_LIST_SIZE);

// inserts trains sorted descending by their offset away from the node
//
// trains farthest from the node will be closer to the beginning of the list
int tr_at_list_insert(tr_at_list *tr_at, train *tr) {
  int i, r, size;
  train *other;

  size = tr_at->size;
  for (i = 0; i < size; ++i) {
    tr_at_list_pop(tr_at, &other);

    // printf("%s\n", tr->curr_pos.pos->name);
    // printf("%d %d\n", tr, other);
    assert(other != tr); // tr should not be in this list

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
  t->curr_pos.dir = DIR_AHEAD;
  t->curr_pos.ts  = INT_MIN;
  t->next_pos.pos = NULL;
  t->next_pos.dir = 0;
  t->next_pos.ts  = INT_MIN;
  t->next_sen     = NULL;
  // pp_list_init(&t->prev_pos);
}

int est_last_ts(estimator *est) {
  return est->last_ts;
}

train *est_get_train(estimator *est, int tr_num) {
  if (est->tmap[tr_num] == -1)
    return NULL;
  return &est->train[est->tmap[tr_num]];
}

void est_init(estimator *est) {
  int i, j;

  est->ntrains = 0;
  est->last_ts = 0;

  for (i = 0; i < NUM_TRAINS; ++i) {
    train_uninit(&est->train[i]);
  }

  for (i = 0; i < TRACK_MAX; ++i) {
    tr_at_list_init(&est->tr_at[i][0]);
    tr_at_list_init(&est->tr_at[i][1]);
    for (j = 0; j < NUM_TRAINS; ++j) {
      est->crumb[i].crumb[j].train = NULL;
      est->crumb[i].crumb[j].ts = -1;
      est->crumb[i].crumb[j].speed = -1;
      est->crumb[i].num = 0;
    }
  }

  for (i = 0; i < TRAIN_MAX; ++i) {
    est->tmap[i] = -1;
  }

  for (i = 0; i < SENSOR_SIZE; ++i) {
    sen_reg_list_init(&est->sen_reg[i]);
  }

  for (i = 0; i < SWITCH_SIZE; ++i) {
    est->sw[i].state = DIR_CURVED;
    est->sw[i].conf = 50; // we aren't confident
    est->sw[i].last_known_state = DIR_CURVED;   // by default set last known to curved
  }
}

void est_init_trains(estimator *est, int ts, track_node *T, int track) {
  int r;
  pos_event pe;
  pe.ts  = ts;

  if (track == 1) {
    pe.pos = &T[trhr(T, "A1")];
    pe.off = 0;
    est_add_tr(est, 1, &pe);
    // pe.pos = &T[trhr(T, "B8")];
    // pe.off = 10;
    // est_add_tr(est, 77, &pe);
    // pe.pos = &T[trhr(T, "B12")];
    // pe.off = 10;
    // est_add_tr(est, 78, &pe);
    // pe.pos = &T[trhr(T, "B10")];
    // pe.off = 10;
    // est_add_tr(est, 79, &pe);
  }
  else {
    pe.pos = &T[trhr(T, "A1")];
    pe.off = 0;
    r = est_add_tr(est, 77, &pe);
    assert(r == 0);
    pe.pos = &T[trhr(T, "A13")];
    pe.off = 0;
    r = est_add_tr(est, 79, &pe);
    assert(r == 0);
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
  train->curr_pos.dir = DIR_AHEAD;
  train->len = 200;        // TODO: hard code train length to be 20cm
  train->next_sen = NULL;
  getVelocityModel(&train->s_model, train->num);

  tr_at = &est->tr_at[train->curr_pos.pos->id][train->curr_pos.dir];
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

#define SENSOR_DEPTH 2
static train *est_recover_from_crumbs(estimator *est, track_node *node) {
  int i, r;
  int nse[TRACK_MAX];
  bfs_q q;
  track_node *next;
  tn_crumbs *crumbs;
  train_crumb *crumb;
  bfs_q_init(&q);

  node = node->reverse;

  nse[node->id] = 0;
  r = bfs_q_push(&q, node);

  while (q.size > 0) {
    r = bfs_q_pop(&q, &node);
    if (r) return 0;

    crumbs = &est->crumb[node->reverse->id];

    // if there are crumbs, take the crumb with the oldest (smallest) timestamp
    train *min_tr;
    int min_ts;
    if (crumbs->num > 0) {
      min_tr = NULL;
      min_ts = INT_MAX;
      for (i = 0; i < est->ntrains; ++i) {
        crumb = &crumbs->crumb[i];
        if (crumb->train && crumb->ts < min_ts) {
          min_tr = crumb->train;
        }
      }
      assert(min_tr);
      return min_tr;
    }

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      nse[next->id] = nse[node->id];
      r = bfs_q_push(&q, next);
      if (r) return 0;

      next = node->edge[DIR_STRAIGHT].dest;
      nse[next->id] = nse[node->id];
      r = bfs_q_push(&q, next);
      if (r) return 0;
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else if (node->type == NODE_SENSOR) {
      next = node->edge[DIR_AHEAD].dest;
      if (nse[node->id] + 1 > SENSOR_DEPTH)
        continue;
      nse[next->id] = nse[node->id] + 1;
      r = bfs_q_push(&q, next);
      if (r) return 0;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      nse[next->id] = nse[node->id];
      r = bfs_q_push(&q, next);
      if (r) return 0;
    }
  }

  return NULL;
}


// associate a train to a sensor event
static train *est_assoc_tr(estimator *est, pos_event *pe) {
  // int i;
  int dir;
  train *train;
  sen_reg_list *sen_regs;
  tr_at_list *tr_at;
  track_node *prev;

  assert(pe);

  assert(pe->pos->type == NODE_SENSOR);

  train = NULL;
  sen_regs = &est->sen_reg[pe->pos->id];

  // sen_regs holds the trains that have passed the sensor in the model, in the order
  // they passed the sensor
  if (sen_regs->size > 0) {
    sen_reg_list_pop(sen_regs, &train);
    return train;
  }

  // get the tr_at of the track_node before the sensor

  prev = pe->pos->reverse->edge[DIR_AHEAD].dest->reverse;
  dir = tn_get_dir(prev, pe->pos);
  assert(dir != -1);
  assert(dir == DIR_STRAIGHT || dir == DIR_CURVED);
  // assert(prev->type != NODE_BRANCH);
  tr_at = &est->tr_at[prev->id][dir];
  assert(prev->edge[dir].dist > 0);

  if (tr_at->size > 0) {
    tr_at_list_get(tr_at, 0, &train);
    return train;
  }

  train = est_recover_from_crumbs(est, pe->pos);

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
  else if (sw->conf == 50) {
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
static int est_next_node(estimator *est, train *train, track_node **ret_next, int *ret_dir) {
  int off, dist, dir, next_dir;
  track_node *cur, *next;

  dist = -1;
  dir = DIR_AHEAD;
  next = NULL;

  cur = train->curr_pos.pos;
  off = train->curr_pos.off;
  dir = train->curr_pos.dir;

  assert(dir == DIR_STRAIGHT || dir == DIR_CURVED);

  next = cur->edge[dir].dest;
  dist = cur->edge[dir].dist - off;
  assert(next);

  switch (next->type) {
    case NODE_BRANCH:
      next_dir = est_estimate_sw_dir(est, next->num);
      break;
    case NODE_EXIT:
      *ret_dir = -1;
      next = NULL;
      return dist;
    default:
      next_dir = DIR_AHEAD;
      break;
  }

  *ret_next = next;
  *ret_dir = next_dir;
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

  assert(tr->curr_pos.dir == 0 || tr->curr_pos.dir == 1);
  tr_at = &est->tr_at[tr->curr_pos.pos->id][tr->curr_pos.dir];
  r = tr_at_list_rem(tr_at, tr);
  assert(r == 0);

  tr->curr_pos.pos = dest;
  tr->curr_pos.off = 0;
  tr->curr_pos.dir = DIR_AHEAD;

  assert(tr->curr_pos.dir == 0 || tr->curr_pos.dir == 1);
  tr_at = &est->tr_at[tr->curr_pos.pos->id][tr->curr_pos.dir];
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
  int i, r, dir, dist_to_next;
  track_node *next;
  train *other;
  tr_at_list *tr_at;
  pos_event pos_orig;

  pos_orig = tr->curr_pos;
  assert(tr->curr_pos.dir == 0 || tr->curr_pos.dir == 1);
  tr_at = &est->tr_at[tr->curr_pos.pos->id][tr->curr_pos.dir];

  while (dist_to_move > 0) {
    if (tr_at->size > 0) {
      // iterate through the tr_at list descending (closest trains first)
      // there are other trains on this track node, determine if we can move
      for (i = tr_at->size-1; i >= 0; --i) {
        tr_at_list_get(tr_at, i, &other);
        if (other == tr)
          continue;

        assert(other->curr_pos.pos == tr->curr_pos.pos);

        // check if trains will collide
        if (tr->curr_pos.off < other->curr_pos.off &&
            tr->curr_pos.off + dist_to_move >= other->curr_pos.off) {
          // move train to right behind the blocking train
          if (force) {
            assert(tr->curr_pos.dir == 0 || tr->curr_pos.dir == 1);
            tr_at = &est->tr_at[tr->curr_pos.pos->id][tr->curr_pos.dir];
            r = tr_at_list_rem(tr_at, tr);
            assert(r == 0);
            assert(other->curr_pos.off-1 > 0);

            dist_to_move -= other->curr_pos.off-1 - tr->curr_pos.off;
            tr->curr_pos.off = other->curr_pos.off-1;

            assert(tr->curr_pos.dir == 0 || tr->curr_pos.dir == 1);
            tr_at = &est->tr_at[tr->curr_pos.pos->id][tr->curr_pos.dir];
            r = tr_at_list_insert(tr_at, tr);
            assert(r == 0);
            return dist_to_move;
          }
          else {
            // restore the old state
            tr->curr_pos = pos_orig;
            return -1;
          }
        }
      }
    }

    dist_to_next = est_next_node(est, tr, &next, &dir);
    if (dist_to_next < 0) {
      assert(0);
      // restore the old state
      tr->curr_pos = pos_orig;
      return -2;
    }
    assert(next != NULL);
    assert((next->type == NODE_BRANCH && dir == DIR_CURVED) || (next->type != NODE_BRANCH && dir == DIR_AHEAD));
    assert(dir == DIR_AHEAD || dir == DIR_STRAIGHT || dir == DIR_CURVED);

    // printf("togo %d %s %d\n", dist_to_move, tr->curr_pos.pos->name, tr->curr_pos.off);

    // update tr_at of cur and next maintaining ordering
    r = tr_at_list_rem(tr_at, tr);
    assert(r == 0);

    // move to next node
    // printf("COND %d + %d  >= %d\n", tr->curr_pos.off, dist_to_move, dist_to_next);
    if (dist_to_move >= dist_to_next) {
      // TODO check that next node is not occupied
      dist_to_move -= dist_to_next;
      tr->curr_pos.pos = next;
      tr->curr_pos.dir = dir;
      tr->curr_pos.off = 0;
      assert(tr->curr_pos.dir == 0 || tr->curr_pos.dir == 1);
      tr_at = &est->tr_at[tr->curr_pos.pos->id][tr->curr_pos.dir];
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

// clean up all the crumbs starting at node and working backwards
static int est_cleanup_crumbs(estimator *est, train *tr, track_node *node) {
  int r;
  bfs_q q;
  track_node *next;
  tn_crumbs *crumbs;
  train_crumb *crumb;
  bfs_q_init(&q);

  node = node->reverse;

  r = bfs_q_push(&q, node);

  while (q.size > 0) {
    r = bfs_q_pop(&q, &node);
    if (r) return -1;

    // printf("checking %s for crumbs\n", node->reverse->name);
    crumbs = &est->crumb[node->reverse->id];
    crumb = &crumbs->crumb[est->tmap[tr->num]];
    if (!crumb->train) {
      continue;
    }
    else {
      // printf("cleaning up crumb on %s\n", node->reverse->name);
      crumb->train = NULL;
      crumbs->num--;
    }

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      r = bfs_q_push(&q, next);
      if (r) return -1;

      next = node->edge[DIR_STRAIGHT].dest;
      r = bfs_q_push(&q, next);
      if (r) return -1;
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      r = bfs_q_push(&q, next);
      if (r) return -1;
    }
  }

  return 0;
}

static int est_pass_nodes(estimator *est, train *tr, tn_list *nodes, int ts) {
  int i, r;
  track_node *node;
  tn_crumbs *crumbs;
  train_crumb *crumb;

  // if we passed any sensors then add the train to the sen_reg_list
  // and place crumbs
  for (i = 0; i < nodes->size; ++i) {
    tn_list_get(nodes, i, &node);

    crumbs = &est->crumb[node->id];
    crumb = &crumbs->crumb[est->tmap[tr->num]];

    // assert(crumb->train == NULL);
    // printf("placing crumb at %s\n", node->name);
    crumb->train = tr;
    crumb->ts = ts;
    crumbs->num++;

    // TODO: speed info

    if (node->type == NODE_SENSOR) {
      r = sen_reg_list_rem(&est->sen_reg[node->num], tr);
      r = sen_reg_list_push(&est->sen_reg[node->num], tr);
      assert(r == 0);
    }
  }

  return 0;
}


// advances the train taking into consideration the known switch config
// if while progressing this train we run into a train in front, then
// call `est_update_train` on that train to try to get it out of the way
int est_progress_train(estimator *est, train *tr, int dist_to_move, int ts) {
  int i, dist_rem, r;
  train *other;
  tr_at_list tr_at;
  tn_list nodes;
  swi *sw;

  tr_at_list_init(&tr_at);
  tn_list_init(&nodes);

  sw = est->sw;

  dist_rem = est_move_train_forward(est, tr, dist_to_move, true, &nodes);

  // train was moved up all the way successfully
  if (dist_rem == 0) {
    r = est_pass_nodes(est, tr, &nodes, ts);
    tn_list_init(&nodes);
  }
  // collision with another train, attempt to resolve by updating all trains
  // at the track node and then trying to move again
  else if (dist_rem > 0) {
    assert(tr->curr_pos.dir == 0 || tr->curr_pos.dir == 1);
    tr_at = est->tr_at[tr->curr_pos.pos->id][tr->curr_pos.dir];

    assert(tr_at.size > 0);

    for (i = 0; i < tr_at.size; ++i) {
      tr_at_list_get(&tr_at, i, &other);

      if (other == tr)
        continue;

      est_update_train(est, other, ts);
    }

    // try moving again after the updates to other trains with remaining dist
    dist_rem = est_move_train_forward(est, tr, dist_rem, true, &nodes);

    // move was successful
    if (dist_rem == 0) {
      r = est_pass_nodes(est, tr, &nodes, ts);
      tn_list_init(&nodes);
    }
    else if (dist_rem >= 0) {
      // we cannot move up any further, nothing more can be done for this train
      // TODO: maybe update model?
      return 0;
    }
  }
  // dead end
  else if (dist_rem == -2) {
    assert(0 && "dead-end not handled");
  }
  return 0;
}

track_node *get_next_est_sensor_exc(estimator *est, track_node *node, int *dist) {
  int dir;

  *dist = 0;

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
    *dist += node->edge[dir].dist;
  } while (node->type != NODE_SENSOR);

  return node;
}

int est_update_train(estimator *est, train *train, int ts) {
  int r, delta, dist;

  delta = ts - train->curr_pos.ts;

  if (delta > 0 && train->gear > 0) {
    // move train along the track the corresponding distance for time delta
    // dist = 0; // speed model generated dist traveled in time delta
    // dist = sm_calc_dist(&train->sm, delta);
    dist = (delta * easyInterpolation(&train->s_model, train->gear*10))/1000;
    // printf("%d\n", dist);
    // printf("%d %s %d\n", dist, train->curr_pos.pos->name, train->curr_pos.off);
    r = est_progress_train(est, train, dist, ts);
    if (r) {
    }
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
  assert(ts >= est->last_ts);

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
  int ret, r, rel, ts;
  train *train;

  ret = 0;
  ts = pe->ts;

  // printf("SENSOR HIT @ %s\n", pe->pos->name);
  // update everything to current time to get best accuracy in model adjustments
  r = est_update(est, ts);
  if (r) {
    assert(0 && "TODO");
  }

  // figure out which train this event relates to
  train = est_assoc_tr(est, pe);

  if (train) {

    // 1. figure out where train is relative to sensor
    rel = est_find_train_rel_to_sensor(est, train, pe->pos);

    // 2. update model with knowledge of how far off train is
    if (rel == INT_MAX) {
      ret = -1;
      // assert(0 && "train not found relative to sensor");
    }
    else if (rel < 0) {
      ret = -2;
      // the train is before the sensor
      // printf("BEFORE %d\n", rel);
    }
    else /* if (rel > 0) */ {
      ret = -3;
      // the train is after the sensor
      // printf("AFTER %d\n", rel);
    }

    r = est_cleanup_crumbs(est, train, train->curr_pos.pos);
    assert(r == 0);

    // 3. move train to sensor
    r = est_move_train_to_node_unsafe(est, train, pe->pos);
    r = est_update_train(est, train, ts);
  }
  else {
    ret = -4;
    // assert(0 && "failed to assoc a train");
  }

  return ret;
}


int est_update_tr_gear(estimator *est, int tr_num, int gear, int ts) {
  train *train;

  est_update(est, ts);

  train = &est->train[est->tmap[tr_num]];
  // printf("%d %d\n", train->num, tr_num);
  assert(train->num == tr_num);

  // TODO: initiate some acceleration or deceleration here
  train->gear = gear;
  // sm->start_gear = sm->curr_gear
  // sm->stop_gear = gear
  //
  // sm_acc(

  return 0;
}


// update the estimator with a switch change event
int est_update_sw(estimator *est, int sw_num, int state, int ts) {
  swi *sw;
  sw = &est->sw[sw_num];

  if (sw->conf >= 50) {
    est->sw[sw_num].state = state;
  }

  return est_update(est, ts);
}
