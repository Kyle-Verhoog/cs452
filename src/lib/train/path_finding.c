#include <lib/train/path_finding.h>

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
