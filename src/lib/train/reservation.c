#include <lib/train/reservation.h>

CIRCULAR_BUFFER_DEF(tn_q, track_node *, PATHER_Q_SIZE);

void reserv_init(reservation *r) {
  r->train_num = -1;
  r->reserved = false;
}

void pather_init(pather *p, track_node *TRACK) {
  int i;

  for (i = 0; i < TRACK_MAX; ++i) {
    p->track[i] = TRACK[i];
    p->track[i].reserver = -1;
    reserv_init(&p->reserv[i]);
  }

  for (i = 0; i < TRAIN_MAX; ++i) {
    p->nreservs[i] = 0;
  }
}

void get_all_nodes_in_dist(track_node *node, int max_dist, tn_q *nodes) {
  int r;
  int dist[TRACK_MAX];
  track_node *next;
  tn_q q;

  tn_q_init(nodes);
  tn_q_init(&q);

  dist[node->id] = 0;
  r = tn_q_push(&q, node);

  while (q.size > 0) {
    r = tn_q_pop(&q, &node);
    assert(r == 0);

    if (dist[node->id] > max_dist)
      continue;

    tn_q_push(nodes, node);

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      r = tn_q_push(&q, next);
      dist[next->id] = node->edge[DIR_CURVED].dist + dist[node->id];
      assert(r == 0);
      next = node->edge[DIR_STRAIGHT].dest;
      r = tn_q_push(&q, node->edge[DIR_STRAIGHT].dest);
      dist[next->id] = node->edge[DIR_STRAIGHT].dist + dist[node->id];
      assert(r == 0);
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      r = tn_q_push(&q, node->edge[DIR_AHEAD].dest);
      dist[next->id] = node->edge[DIR_AHEAD].dist + dist[node->id];
      assert(r == 0);
    }
  }

  return;
}


// gets the nodes in the reserved region ceil'd to the next sensor
void get_all_to_sen_in_dist(track_node *node, int min_dist, tn_q *nodes) {
  int r;
  int dist[TRACK_MAX];
  track_node *next;
  tn_q q;

  tn_q_init(nodes);
  tn_q_init(&q);

  dist[node->id] = 0;
  r = tn_q_push(&q, node);

  while (q.size > 0) {
    r = tn_q_pop(&q, &node);
    assert(r == 0);


    if (node->type == NODE_SENSOR && min_dist < dist[node->id]) {
      continue;
    }

    tn_q_push(nodes, node);

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      dist[next->id] = node->edge[DIR_CURVED].dist + dist[node->id];
      r = tn_q_push(&q, next);
      assert(r == 0);

      next = node->edge[DIR_STRAIGHT].dest;
      r = tn_q_push(&q, next);
      dist[next->id] = node->edge[DIR_STRAIGHT].dist + dist[node->id];
      assert(r == 0);
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      r = tn_q_push(&q, next);
      dist[next->id] = node->edge[DIR_AHEAD].dist + dist[node->id];
      assert(r == 0);
    }
  }

  return;
}


int reservation_reserve(reservation *r, int trn) {
  // assert(!r->reserved);
  // assert(r->train_num == -1);
  r->reserved = true;
  r->train_num = trn;
  return 0;
}

int reservation_free(reservation *r, int trn) {
  // assert(r->reserved);
  // assert(r->train_num == trn);
  r->reserved = false;
  r->train_num = -1;
  return 0;
}


int pather_reserve_node(pather *p, int trn, track_node *tn) {
  reservation *r;
  r = &p->reserv[tn->id];

  if (r->reserved && r->train_num != trn) {
    // another train has reserved the location
    return -1;
  }
  else if (r->reserved) {
    // we already reserved this position for the train, ignore
  }
  else {
    if (tn->type == NODE_BRANCH) {
      tn->reserver = trn;
      reservation_reserve(r, trn);

      // have to reserve both corresponding edges to merges
      tn->edge[DIR_STRAIGHT].dest->reverse->reserver = trn;
      r = &p->reserv[tn->edge[DIR_STRAIGHT].dest->reverse->id];
      reservation_reserve(r, trn);

      // reserve the curved edge
      tn->edge[DIR_CURVED].dest->reverse->reserver = trn;
      r = &p->reserv[tn->edge[DIR_CURVED].dest->reverse->id];
      reservation_reserve(r, trn);
    }
    else if (tn->type == NODE_EXIT) {
      tn->reserver = trn;
      reservation_reserve(r, trn);
    }
    else if (tn->edge[DIR_AHEAD].dest->type == NODE_MERGE) {
      // reserve the branch
      tn->edge[DIR_AHEAD].dest->reverse->reserver = trn;
      r = &p->reserv[tn->edge[DIR_AHEAD].dest->reverse->id];
      reservation_reserve(r, trn);

      // reserve the two merges
      tn->edge[DIR_AHEAD].dest->reverse->edge[DIR_CURVED].dest->reverse->reserver = trn;
      r = &p->reserv[tn->edge[DIR_AHEAD].dest->reverse->edge[DIR_CURVED].dest->reverse->id];
      reservation_reserve(r, trn);
      tn->edge[DIR_AHEAD].dest->reverse->edge[DIR_STRAIGHT].dest->reverse->reserver = trn;
      r = &p->reserv[tn->edge[DIR_AHEAD].dest->reverse->edge[DIR_STRAIGHT].dest->reverse->id];
      reservation_reserve(r, trn);
    }
    else {
      tn->reserver = trn;
      reservation_reserve(r, trn);
      tn->edge[DIR_AHEAD].dest->reverse->reserver = trn;
      r = &p->reserv[tn->edge[DIR_AHEAD].dest->reverse->id];
      reservation_reserve(r, trn);
    }
    p->nreservs[trn]++;
  }
  return 0;
}

int pather_free_node(pather *p, int trn, track_node *tn) {
  reservation *r;
  r = &p->reserv[tn->id];

  if (r->reserved && r->train_num != trn) {
    // another train has reserved the location
    return -1;
  }
  else if (!r->reserved) {
    // this position has already been freed
    return -2;
  }
  else {
    if (tn->type == NODE_BRANCH) {
      tn->reserver = -1;
      reservation_free(r, trn);

      // have to reserve both corresponding edges to merges
      tn->edge[DIR_STRAIGHT].dest->reverse->reserver = -1;
      r = &p->reserv[tn->edge[DIR_STRAIGHT].dest->reverse->id];
      reservation_free(r, trn);

      // reserve the curved edge
      tn->edge[DIR_CURVED].dest->reverse->reserver = -1;
      r = &p->reserv[tn->edge[DIR_CURVED].dest->reverse->id];
      reservation_free(r, trn);
    }
    else if (tn->type == NODE_EXIT) {
      tn->reserver = -1;
      reservation_free(r, trn);
    }
    else if (tn->edge[DIR_AHEAD].dest->type == NODE_MERGE) {
      // reserve the branch
      tn->edge[DIR_AHEAD].dest->reverse->reserver = -1;
      r = &p->reserv[tn->edge[DIR_AHEAD].dest->reverse->id];
      reservation_free(r, trn);

      // reserve the two merges
      tn->edge[DIR_AHEAD].dest->reverse->edge[DIR_CURVED].dest->reverse->reserver = -1;
      r = &p->reserv[tn->edge[DIR_AHEAD].dest->reverse->edge[DIR_CURVED].dest->reverse->id];
      reservation_free(r, trn);
      tn->edge[DIR_AHEAD].dest->reverse->edge[DIR_STRAIGHT].dest->reverse->reserver = -1;
      r = &p->reserv[tn->edge[DIR_AHEAD].dest->reverse->edge[DIR_STRAIGHT].dest->reverse->id];
      reservation_free(r, trn);
    }
    else {
      tn->reserver = -1;
      reservation_free(r, trn);
      tn->edge[DIR_AHEAD].dest->reverse->reserver = -1;
      r = &p->reserv[tn->edge[DIR_AHEAD].dest->reverse->id];
      reservation_free(r, trn);
    }
    p->nreservs[trn]--;
  }
  return 0;
}

// returns:
//  -1 if the distance cannot be reserved for the distance
//  0 on success
int pather_reserve_to_sensor(pather *p, int trn, track_node *next_pos, int dist) {
  int i;
  tn_q nodes;
  track_node *tn;

  get_all_to_sen_in_dist(next_pos, dist, &nodes);

  for (i = 0; i < nodes.size; ++i) {
    tn_q_get(&nodes, i, &tn);

    if (pather_reserve_node(p, trn, tn)) {
      return -1;
    }
  }

  return 0;
}


// returns:
//  -1 if the distance cannot be reserved for the distance
//  0 on success
int pather_reserve(pather *p, int trn, track_node *next_pos, int dist) {
  int i;
  tn_q nodes;
  track_node *tn;
  reservation *r;

  get_all_nodes_in_dist(next_pos, dist, &nodes);

  for (i = 0; i < nodes.size; ++i) {
    tn_q_get(&nodes, i, &tn);
    r = &p->reserv[tn->id];
    if (r->reserved && r->train_num != trn) {
      // another train has reserved the location
      return -1;
    }
    else if (r->reserved) {
      // we already reserved this position for the train, ignore
    }
    else {
      r->reserved = true;
      r->train_num = trn;
      p->nreservs[trn]++;
      // printf("%s\n", tn->name);
    }
  }

  return 0;
}



// gets all nodes that are reserved, that do not follow the given node
void pather_get_all_res_nodes(pather *p, track_node *node, track_node *ignore, tn_q *nodes) {
  int r;
  track_node *prev[TRACK_MAX];
  track_node *next;
  tn_q q;

  tn_q_init(nodes);
  tn_q_init(&q);

  prev[node->id] = NULL;
  r = tn_q_push(&q, node);

  while (q.size > 0) {
    r = tn_q_pop(&q, &node);
    assert(r == 0);

    if (node == ignore || !p->reserv[node->id].reserved)
      continue;

    tn_q_push(nodes, node);

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      r = tn_q_push(&q, next);
      prev[next->id] = node;
      assert(r == 0);
      next = node->edge[DIR_STRAIGHT].dest;
      r = tn_q_push(&q, next);
      prev[next->id] = node;
      assert(r == 0);
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      r = tn_q_push(&q, next);
      prev[next->id] = node;
      assert(r == 0);
    }
  }

  return;
}

int pather_free_nodes(pather *p, int trn, track_node *node) {
  int r, ret;
  track_node *prev[TRACK_MAX];
  track_node *next;
  tn_q q;

  ret = 0;
  tn_q_init(&q);

  prev[node->id] = NULL;
  r = tn_q_push(&q, node);

  while (q.size > 0) {
    r = tn_q_pop(&q, &node);
    assert(r == 0);

    if (!p->reserv[node->id].reserved)
      continue;

    r = pather_free_node(p, trn, node);
    if (r) ret = r;

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      r = tn_q_push(&q, next);
      prev[next->id] = node;
      assert(r == 0);
      next = node->edge[DIR_STRAIGHT].dest;
      r = tn_q_push(&q, next);
      prev[next->id] = node;
      assert(r == 0);
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      r = tn_q_push(&q, next);
      prev[next->id] = node;
      assert(r == 0);
    }
  }

  return ret;
}


// make the assumption reservations are contiguous and free backwards
// (including) the given node
int pather_free_before(pather *p, int trn, track_node *node) {
  if (!p->reserv[node->id].reserved)
    return -1;

  return pather_free_nodes(p, trn, node->reverse);
}


static void path_find(track_node *track, track_node *s, track_node *d, int *prev, int trnum) {
  int u, v, i;
  int sid, did;
  int dist[TRACK_MAX];
  int vist[TRACK_MAX];
  tp_queue q;


  int nedges[6];
  nedges[NODE_NONE]   = 0;
  nedges[NODE_SENSOR] = 1;
  nedges[NODE_BRANCH] = 2;
  nedges[NODE_MERGE]  = 1;
  nedges[NODE_ENTER]  = 1;
  nedges[NODE_EXIT]   = 0;

  sid = s->id;
  did = d->id;
  tpq_init(&q);


  for (v = 0; v < TRACK_MAX; ++v) {
    dist[v] = INT_MAX;
    prev[v] = -1;
    vist[v] = 0;
  }

  dist[sid] = 0;
  tpq_add(&q, sid, dist[sid]);

  while (q.size > 0) {
    u = tpq_pop(&q);

    if (u == did)
      break;

    vist[u] = 1;
    int alt;
    track_edge *e;

    if (track[u].reserver != -1 && track[u].reserver != trnum)
      assert(0);

    for (i = 0; i < nedges[track[u].type]; ++i) {
      e = &track[u].edge[i];

      v = e->dest->id;

      alt = dist[u] + e->dist;

      if (!vist[v] && alt < dist[v]) {
        dist[v] = alt;
        prev[v] = u;
        tpq_add(&q, v, alt);
      }
    }
  }
}

static int pather_generate(path *p, int trn) {
  int i, n, sid, eid;
  int buf[TRACK_MAX];

  sid = p->start->id;
  eid = p->end->id;

  path_find(p->track, p->start, p->end, p->pred, trn);

  n = 0;
  buf[n++] = eid;

  i = p->pred[eid];
  while (i != -1) {
    buf[n++] = i;
    i = p->pred[i];
  }

  for (n = n-1; n >= 0; --n) {
    tr_path_push(&p->ahead, &p->track[buf[n]]);
  }

  if (p->ahead.size < 2) {
    return -1;
  }

  p->ready = true;
  return 0;
}


int pather_path(pather *pr, path *p, int trn) {
  p->track = pr->track;
  return pather_generate(p, trn);
}
