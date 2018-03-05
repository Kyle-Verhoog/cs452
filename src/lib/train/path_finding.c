#include <lib/train/path_finding.h>


// Bellman-Ford path finding
void bf_path_find(track_node *s, track_node *d) {
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
}
