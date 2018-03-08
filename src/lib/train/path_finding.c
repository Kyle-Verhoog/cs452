#include <lib/train/path_finding.h>

CIRCULAR_BUFFER_DEF(tr_path, track_node*, TRACK_MAX);
CIRCULAR_BUFFER_DEF(sw_look_ahead, track_node*, TRACK_MAX);

#ifndef X86
#include <user/terminal_manager.h>
int generate_train_path(TrainDescriptor *tr, track_node *track, int sid, int eid) {
  int i, n;
  int buf[TRACK_MAX];

  dij_path_find(track, &track[sid], &track[eid], tr->path_prev);

  tid_t tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  i = tr->path_prev[eid];
  n = 0;
  while (i != sid && i != -1) {
    // TMLogStrf(tm_tid, "");
    buf[n++] = i;
    i = tr->path_prev[eid];
  }

  assert(i == sid);

  if (i == -1)
    assert(0 && "no path found");

  for (n = n-1; n >= 0; --n) {
    tr_path_push(&tr->path, buf[n]);
  }
}
#else
#include <assert.h>
#endif

void path_init(path *p, track_node *track) {
  p->current = NULL;
  p->start   = NULL;
  p->end     = NULL;
  p->ready   = false;
  p->active  = false;
  p->track = track;
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

// return the switches in the next `dist` along the path
void path_switches_in_next_dist(path *p, int dist) {
}
// find the next n switches on the path
void path_next_n_switches(path *p, int n, sw_look_ahead *sw_la) {
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
