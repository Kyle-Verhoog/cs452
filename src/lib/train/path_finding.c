#include <lib/train/path_finding.h>

CIRCULAR_BUFFER_DEF(tr_path, track_node*, TRACK_MAX);
CIRCULAR_BUFFER_DEF(sw_configs, sw_config, TRACK_MAX);

void path_init(path *p, track_node *track) {
  p->current = NULL;
  p->start   = NULL;
  p->end     = NULL;
  p->ready   = false;
  p->active  = false;
  p->track = track;
  p->path_len = 0;
  tr_path_init(&p->ahead);
  tr_path_init(&p->behind);
}

track_node *path_peak_next(path *p) {
  return NULL;
}

void path_next(path *p) {
  assert(p->active);
  int r;
  track_node *next;
  next = NULL;

  // push the current into behind
  if (p->current != NULL) {
    r = tr_path_push(&p->behind, p->current);
    assert(r == 0);
    p->current = NULL;
  }

  // load next current from ahead
  if (p->ahead.size > 0) {
    r = tr_path_pop(&p->ahead, &next);
    assert(r == 0);
    p->current = next;
  }

  if (p->current == NULL && p->ahead.size == 0) {
    p->active = false;
  }
}

// follow path until node t
int path_follow_to(path *p, track_node *t) {
  int i, r;
  track_node *tn;

  // check that t is actually in the path ahead
  for (i = 0; i < p->ahead.size; ++i) {
    r = tr_path_get(&p->ahead, i, &tn);
    assert(r == 0);
    if (tn == t) {
      break;
    }
  }

  // t not in path ahead
  if (tn != t) {
    assert(0);
    return -1;
  }

  for (; i >= 0; --i) {
    path_next(p);
  }
  return 0;
}

// backtrack on the path to t
int path_backtrack(path *p, track_node *t) {
  return 0;
}

int path_get_switch_config(track_node *t1, track_node *t2) {
  assert(t1->type == NODE_BRANCH);
  track_edge *e;
  e = &t1->edge[DIR_STRAIGHT];
  if (e->dest == t2) {
    return DIR_STRAIGHT;
  }
  e = &t1->edge[DIR_CURVED];
  if (e->dest == t2) {
    return DIR_CURVED;
  }
  assert(0 && "nodes are not connected via edge");
}

int track_node_dist(track_node *t1, track_node *t2) {
  assert(t1->type != NODE_EXIT);
  int dir;
  dir = DIR_AHEAD;
  if (t1->type == NODE_BRANCH) {
    dir = path_get_switch_config(t1, t2);
  }
  return t1->edge[dir].dist;
}

void path_to_str(path *p, char *buf) {
  int i, offset, x;
  track_node *tn;
  x = offset = 0;
  for (i = 0; i < p->ahead.size; ++i) {
    tr_path_get(&p->ahead, i, &tn);
    offset += buf_pack_str(buf+offset, tn->name);
    offset += buf_pack_c(buf+offset, ' ');
    if (x == 6) {
      offset += buf_pack_str(buf+offset, "\n    ");
      x = 0;
    }
    else {
      x++;
    }
  }
  offset += buf_pack_c(buf+offset, '\0');
}

// return the switches in the next `dist` along the path
void path_switches_in_next_dist(path *p, sw_configs *sw_cfgs, int dist) {
  assert(p->active);
  int i, d;
  track_node *cur, *next;

  d = 0;
  cur = p->current;
  for (i = 0; i < p->ahead.size; ++i) {
    tr_path_get(&p->ahead, i, &next);

    int dir;
    sw_config sw_cfg;
    if (cur->type == NODE_BRANCH) {
      dir = path_get_switch_config(cur, next);
      sw_cfg.sw = cur;
      sw_cfg.state_required = dir;
      sw_configs_push(sw_cfgs, sw_cfg);
    }

    d += track_node_dist(cur, next);
    if (d >= dist)
      break;

    cur = next;
  }
  // printf("%d\n", d);
}
// find the next n switches on the path
void path_next_n_switches(path *p, int n, sw_configs *sw_cfgs) {
}

// find the next switches before the next sensor
void path_switches_before_next_sensor(path *p) {
}

void path_set_destination(path *p, track_node *start, track_node *end) {
  p->start = start;
  p->end = end;
  p->current = NULL;
}

int path_generate(path *p) {
  int i, n, sid, eid;
  int buf[TRACK_MAX];

  assert(!p->active);
  assert(p->start != NULL);
  assert(p->end != NULL);
  assert(p->start != p->end);

  sid = p->start->id;
  eid = p->end->id;

  dij_path_find(p->track, p->start, p->end, p->pred);
  assert(p->pred[sid] == -1);

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

  if (p->ahead.size < 1) {
    return -1;
  }

  p->ready = true;
  return 0;
}

void path_start(path *p, track_node *start_node) {
  assert(p->start != NULL);
  assert(p->end != NULL);
  if (start_node != p->start) {
    // should be a warning probably, assert for now
    assert(0);
  }
  p->active = true;
  path_follow_to(p, start_node);
}

void dij_path_find(track_node *track, track_node *s, track_node *d, int *prev) {
  int u, v, i;
  int sid, did;
  int dist[TRACK_MAX];
  // int prev[TRACK_MAX];
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
    for (i = 0; i < nedges[track[u].type]; ++i) {
      // assert(&track[u].edge[i]);
      e = &track[u].edge[i];
      // assert(e->src == &track[u]);
      // assert(e->dist < 1000 && e->dist >= 0);
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
