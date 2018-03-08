#include <test/train/path_finding.h>

#define T(str) trhr(t, str)

static void trhr_test(track_node *t) {
  assert(trhr(t, "BR15") == 108);
  assert(trhr(t, "MR15") == 109);
  assert(trhr(t, "EX1") == 125);
}

static void dij_no_path(track_node *t) {
  track_node *start, *dest;
  int prev[TRACK_MAX];
  // 127->120 should fail
  start = &t[127];
  dest  = &t[120];
  dij_path_find(t, start, dest, prev);
  assert(prev[120] == -1);
}

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

static void dij_branch_path(track_node *t) {
  track_node *start, *dest;
  int prev[TRACK_MAX];
  start = &t[76];
  dest  = &t[73];
  dij_path_find(t, start, dest, prev);
  // printf("%d\n", prev[73]);
  assert(prev[73] == 94);
  // printf("%d\n", prev[73]);
  assert(prev[94] == 97);
  assert(prev[97] == 53);
  assert(prev[53] == 68);
  assert(prev[68] == 99);
  assert(prev[99] == 67);
  assert(prev[67] == 49);
  assert(prev[49] == 120);
  assert(prev[120] == 122);
  assert(prev[122] == 119);
  assert(prev[119] == 28);
  assert(prev[28] == 62);
  assert(prev[62] == 112);
  assert(prev[112] == 76);

  start = &t[T("C4")];
  dest  = &t[T("E14")];
  dij_path_find(t, start, dest, prev);
  assert(prev[T("E14")] == T("MR17"));
  assert(prev[T("MR17")] == T("D16"));
  assert(prev[T("D16")] == T("B14"));
  assert(prev[T("B14")] == T("BR154"));
  assert(prev[T("BR154")] == T("MR156"));
  assert(prev[T("MR156")] == T("E1"));
  assert(prev[T("E1")] == T("E16"));
  assert(prev[T("E16")] == T("BR13"));
  assert(prev[T("BR13")] == T("C11"));
  assert(prev[T("C11")] == T("BR14"));
  assert(prev[T("BR14")] == T("A3"));
  assert(prev[T("A3")] == T("B15"));
  assert(prev[T("B15")] == T("MR15"));
  assert(prev[T("MR15")] == T("C6"));
  assert(prev[T("C6")] == T("MR6"));
  assert(prev[T("MR6")] == T("BR18"));
  assert(prev[T("BR18")] == T("MR5"));
  assert(prev[T("MR5")] == T("C4"));
}

static void init_path(track_node *t) {
  path p;
  path_init(&p, t);

}

static void generate_simple_path(track_node *t) {
  track_node *start, *end;
  path p;
  start = &t[0];
  end  = &t[103];
  path_init(&p, t);
  path_set_destination(&p, start, end);
  path_generate(&p);
  assert(p.ready);
  assert(p.current == NULL);
  assert(p.behind.size == 0);
  assert(p.ahead.size == 2);
  assert(p.ahead.buf[0] == start);
  assert(p.ahead.buf[1] == end);

  path_start(&p, start);
  assert(p.current == start);
  assert(p.behind.size == 0);
  assert(p.ahead.size == 1);

  path_next(&p);
  assert(p.behind.size == 1);
  assert(p.ahead.size == 0);
  assert(p.current == end);

  path_next(&p);
  assert(p.behind.size == 2);
  assert(p.ahead.size == 0);
  assert(p.current == NULL);
}

static void generate_longer_path(track_node *t) {
  track_node *start, *end;
  path p;
  path_init(&p, t);
  // start = &track[24];
  // dest  = &track[74];
  // dij_path_find(track, start, dest, prev);
  // assert(prev[74] == 93);
  // assert(prev[93] == 88);
  // assert(prev[88] == 115);
  // assert(prev[115] == 38);
  // assert(prev[38] == 85);
  // assert(prev[85] == 4);
  // assert(prev[4] == 24);
}

void train_pathing_tests() {
  track_node track[TRACK_MAX];
  init_tracka(track);
  assert(track[0].num == 0);
  assert(track[0].reverse == &track[1]);

  trhr_test(track);
  dij_no_path(track);
  dij_simple_path(track);
  dij_branch_path(track);
  init_path(track);
  generate_simple_path(track);
}
