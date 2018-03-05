#include <test/train/path_finding.h>

static void dij_simple_path(track_node *track) {
  track_node *start, *dest;
  start = &track[0];
  dest  = &track[1];

  dij_path_find(track, start, dest);
}

static void bf_simple_path(track_node *track) {
  track_node *start, *dest;
  start = &track[0];
  dest  = &track[1];

  bf_path_find(start, dest);
}

void train_pathing_tests() {
  track_node track[TRACK_MAX];
  init_tracka(track);
  assert(track[0].num == 0);
  assert(track[0].reverse == &track[1]);

  bf_simple_path(track);
}
