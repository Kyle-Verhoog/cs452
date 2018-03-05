#include <test/train/path_finding.h>

static void dij_simple_path(track_node *track) {
  track_node *start, *dest;
  int prev[TRACK_MAX];
  start = &track[0];
  dest  = &track[103];
  assert(start->id == 0);
  assert(dest->id == 103);

  dij_path_find(track, start, dest, prev);
  assert(prev[103] == 0);

  start = &track[24];
  dest  = &track[74];
  dij_path_find(track, start, dest, prev);
  assert(prev[74] == 93);
  assert(prev[93] == 88);
  assert(prev[88] == 115);
  assert(prev[115] == 38);
  assert(prev[38] == 85);
  assert(prev[85] == 4);
  assert(prev[4] == 24);
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

  dij_simple_path(track);
}
