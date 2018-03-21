#include <test/train/path_finding.h>
#include <lib/train/train_defines.h>

#define T(str) trhr(t, str)

typedef enum SwitchState{
	SW_STRAIGHT = 33,
	SW_CURVE = 34
} SwitchState;

typedef struct Switch{
  SwitchState state;
  track_node *branch;
  track_node *merge;
}Switch;

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

static void path_switches(track_node *t) {
  track_node *start, *end;
  path p;
  path_init(&p, t);

  start = &t[T("A1")];
  end   = &t[T("E12")];

  path_set_destination(&p, start, end);
  path_generate(&p);
  assert(p.ready);
  assert(p.current == NULL);
  assert(p.behind.size == 0);
  assert(p.ahead.size == 10);

  path_start(&p, start);
  sw_configs cfgs;
  sw_config cfg;
  sw_configs_init(&cfgs);
  path_switches_in_next_dist(&p, &cfgs, 2500);
  assert(cfgs.size == 1);
  sw_configs_pop(&cfgs, &cfg);
  assert(cfg.state_required == DIR_STRAIGHT);
  assert(cfg.sw->id == T("BR8"));

  path_init(&p, t);

  start = &t[T("A1")];
  end   = &t[T("E13")];
  path_set_destination(&p, start, end);
  path_generate(&p);
  assert(p.ready);
  assert(p.current == NULL);
  assert(p.behind.size == 0);
  assert(p.ahead.size == 10);

  path_start(&p, start);
  sw_configs_init(&cfgs);
  path_switches_in_next_dist(&p, &cfgs, 2500);
  assert(cfgs.size == 1);
  sw_configs_pop(&cfgs, &cfg);
  assert(cfg.state_required == DIR_CURVED);
  assert(cfg.sw->id == T("BR8"));
}

static void follow_path(track_node *t) {
  track_node *start, *end;
  path p;
  int r;
  path_init(&p, t);

  start = &t[T("A1")];
  end   = &t[T("A9")];
  path_set_destination(&p, start, end);
  path_generate(&p);
  path_start(&p, p.start);

  r = path_follow_to(&p, &t[T("C13")]);
  assert(r == 0);
  assert(p.current == &t[T("C13")]);
  assert(p.behind.size == 3);
  assert(p.behind.buf[0] == &t[T("A1")]);
  assert(p.behind.buf[1] == &t[T("MR12")]);
  assert(p.behind.buf[2] == &t[T("MR11")]);

  r = path_follow_to(&p, &t[T("D7")]);
  assert(r == 0);
  assert(p.current == &t[T("D7")]);
  assert(p.behind.size == 5);
  assert(p.behind.buf[0] == &t[T("A1")]);
  assert(p.behind.buf[1] == &t[T("MR12")]);
  assert(p.behind.buf[2] == &t[T("MR11")]);
  assert(p.behind.buf[3] == &t[T("C13")]);

  sw_configs sw_cfgs;
  sw_configs_init(&sw_cfgs);
  sw_config sw_cfg;
  path_switches_in_next_dist(&p, &sw_cfgs, 500);
  assert(sw_cfgs.size == 1);
  path_switches_in_next_dist(&p, &sw_cfgs, 1000);
  assert(sw_cfgs.size == 2);
}

static void switch_flipping(track_node *t) {
  track_node *start, *end;
  path p;
  int i, r;
  path_init(&p, t);

  start = &t[T("A1")];
  end   = &t[T("A9")];
  path_set_destination(&p, start, end);
  path_generate(&p);

  Switch slist[SWITCH_SIZE];
  int node = 0;
  for(i = NORM_SW_LOW; i <= NORM_SW_HIGH; ++i){
    while(true){
      if(t[node].type == NODE_BRANCH){
        slist[i].branch = &t[node];
        slist[i].merge = &t[node+1];
        node+=2;
        break;
      }
      node++;
    }
    slist[i].state = SW_CURVE;
  }

  track_node *sw_tn;
  sw_configs sw_cfgs;
  sw_configs_init(&sw_cfgs);
  sw_config sw_cfg;
  path_switches_in_next_dist(&p, &sw_cfgs, 5000000);
  for (i = 0; i < sw_cfgs.size; ++i) {
    r = sw_configs_get(&sw_cfgs, sw_cfgs.size-i-1, &sw_cfg);
    assert(r == 0);
    sw_tn = sw_cfg.sw;

    if (sw_cfg.state_required != slist[sw_tn->num].state - 33) {
    }
  }
}

static void print_path(track_node *t) {
  track_node *start, *end;
  path p;
  char buf[1024];
  path_init(&p, t);

  start = &t[T("A1")];
  end   = &t[T("E13")];
  path_set_destination(&p, start, end);
  path_generate(&p);
  path_to_str(&p, buf);
  // printf("%s\n", buf);

  start = &t[T("A3")];
  end   = &t[T("D13")];
  path_init(&p, p.track);
  path_set_destination(&p, start, end);
  path_generate(&p);
  path_to_str(&p, buf);
  // printf("%s\n", buf);


  start = &t[T("A3")];
  end   = &t[T("C3")];
  path_init(&p, p.track);
  path_set_destination(&p, start, end);
  path_generate(&p);
  path_to_str(&p, buf);
  // printf("%s\n", buf);

  start = &t[T("A1")];
  end   = &t[T("A9")];
  path_init(&p, p.track);
  path_set_destination(&p, start, end);
  path_generate(&p);
  path_to_str(&p, buf);

  start = &t[T("A1")];
  end   = &t[T("A1")];
  path_init(&p, p.track);
  path_set_destination(&p, start, end);
  path_generate(&p);
  path_to_str(&p, buf);
  // printf("%s\n", buf);
}

static void generate_longer_path(track_node *t) {
  // track_node *start, *end;
  // path p;
  // path_init(&p, t);
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
  generate_longer_path(track);
  path_switches(track);
  follow_path(track);
  print_path(track);
  switch_flipping(track);
}
