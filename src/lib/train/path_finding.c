#include <lib/train/path_finding.h>
#include <assert.h>

// Bellman-Ford path finding
void bf_path_find(track_node *s, track_node *d) {
  /*
  int i;
  int snum, dnum;

  snum = s->num;
  dnum = d->num;

  int dist[TRACK_MAX];
  track_node *pred[TRACK_MAX];

  dist[snum] = 0;

  for (i = 0; i < TRACK_MAX; ++i) {
    dist[i] = INT_MAX;
    pred[i] = 0;
  }
  */
}


void dij_path_find(track_node *track, track_node *s, track_node *d) {
  int u, v, i;
  int snum, dnum;
  int dist[TRACK_MAX];
  int *prev[TRACK_MAX];
  tp_queue q;

  snum = s->num;
  dnum = d->num;
  tpq_init(&q);

  dist[snum] = 0;

  for (v = 0; v < TRACK_MAX; ++v) {
    if (v != snum) {
      dist[v] = INT_MAX;
      prev[v] = -1;
    }
    tpq_add(&q, v, dist[v]);
  }

  while (q.size > 0) {
    u = tpq_pop(&q);

    for (i = 0; i < track[u].type == NODE_BRANCH ? 2 : 1; ++i) {
      track_edge *e;
      int alt;
      e = &track[u].edge[i];
      v = e->dest->num;
      alt = dist[u] + e->dist;
      if (alt < dist[v]) {
        dist[v] = alt;
        prev[v] = u;
        tpq_dec_priority(&q, v, alt);
      }
    }
  }

  assert(prev[dnum] != snum);
}
