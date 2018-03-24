#include <test/train/event_window.h>

static track_node track[TRACK_MAX];

static void ev_wm_init_test() {
  ev_wm wm;
  ev_wm_init(&wm);
}

static void ev_wm_add_to_window_test() {
  ev_wm wm;
  track_node *tn;
  ev_wm_init(&wm);

  tn = &track[trhr(track, "A1")];
  ev_wm_add_to_window(&wm, 0, tn);

  assert(wm.window_map[0] == &wm.window[0]);

  ev_wm_next_window(&wm);

  ev_wm_add_to_window(&wm, 2, tn);
  assert(wm.window_map[2] == &wm.window[1]);


  ev_wm_init(&wm);
  ev_wm_add_to_window(&wm, 0, tn);
  ev_wm_add_to_window(&wm, 1, tn);

  assert(wm.window_map[0] == &wm.window[0]);
  assert(wm.window_map[1] == &wm.window[0]);

  ev_wm_next_window(&wm);

  ev_wm_add_to_window(&wm, 2, tn);
  ev_wm_add_to_window(&wm, 3, tn);
  assert(wm.window_map[2] == &wm.window[1]);
  assert(wm.window_map[3] == &wm.window[1]);
}

static void ev_wm_res_to_window_test() {
  ev_wm wm;
  int r;
  track_node *tn;
  ev_wm_init(&wm);

  tn = &track[trhr(track, "A1")];
  ev_wm_add_to_window(&wm, 0, tn);
  ev_wm_add_to_window(&wm, 1, tn);
  ev_wm_next_window(&wm);

  // test a double hit
  r = ev_wm_res_to_window(&wm, 0, HIT);
  assert(r == 0);
  r = ev_wm_res_to_window(&wm, 1, HIT);
  assert(r == 2);

  tn = NULL;
  tn = ev_wm_get_window_tn(&wm, 1);
  assert(tn == &track[trhr(track, "A1")]);
}


static void ev_wm_delete_test() {
  ev_wm wm;
  int r;
  track_node *tn;
  ev_wm_init(&wm);

  r = ev_wm_delete_if_complete(&wm, 0);
  assert(r == -1);
  r = ev_wm_delete_if_complete(&wm, 3);
  assert(r == -1);

  tn = &track[trhr(track, "A1")];
  ev_wm_add_to_window(&wm, 0, tn);
  ev_wm_add_to_window(&wm, 1, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_delete_if_complete(&wm, 0);
  assert(r == -2);

  r = ev_wm_res_to_window(&wm, 0, HIT);
  assert(r == 0);
  r = ev_wm_res_to_window(&wm, 1, TIMEOUT);
  assert(r == 3);
}

static void sim_normal_events() {
  ev_wm wm;
  int r;
  track_node *tn;
  ev_wm_init(&wm);

  tn = &track[trhr(track, "A1")];
  ev_wm_add_to_window(&wm, 0, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_res_to_window(&wm, 0, HIT);
  assert(r == 3);

  r = ev_wm_delete_if_complete(&wm, 0);
  assert(r == 0);


  tn = &track[trhr(track, "C13")];
  ev_wm_add_to_window(&wm, 1, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_res_to_window(&wm, 1, HIT);
  assert(r == 3);

  r = ev_wm_delete_if_complete(&wm, 1);
  assert(r == 0);


  tn = &track[trhr(track, "E7")];
  ev_wm_add_to_window(&wm, 2, tn);
  ev_wm_next_window(&wm);

  tn = &track[trhr(track, "D7")];
  ev_wm_add_to_window(&wm, 2, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_res_to_window(&wm, 2, TIMEOUT);
  assert(r == 1);

  r = ev_wm_delete_if_complete(&wm, 1);
  assert(r == -1);


  tn = &track[trhr(track, "C1")];
  ev_wm_add_to_window(&wm, 3, tn);
  tn = &track[trhr(track, "C2")];
  ev_wm_add_to_window(&wm, 4, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_res_to_window(&wm, 3, TIMEOUT);
  assert(r == 0);
  r = ev_wm_res_to_window(&wm, 4, TIMEOUT);
  assert(r == 1);

  tn = NULL;
  tn = ev_wm_get_window_tn(&wm, 4);
  assert(tn == &track[trhr(track, "C1")]);

  tn = &track[trhr(track, "E1")];
  ev_wm_add_to_window(&wm, 5, tn);
  tn = &track[trhr(track, "E2")];
  ev_wm_add_to_window(&wm, 6, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_res_to_window(&wm, 5, HIT);
  assert(r == 0);

  tn = &track[trhr(track, "E5")];
  ev_wm_add_to_window(&wm, 7, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_res_to_window(&wm, 6, HIT);
  assert(r == 2);
}

static void sim_double_hit_event() {
  ev_wm wm;
  int r;
  track_node *tn;
  ev_wm_init(&wm);
}

void event_window_tests() {
  init_tracka(track);
  ev_wm_init_test();
  ev_wm_add_to_window_test();
  ev_wm_res_to_window_test();
  ev_wm_delete_test();
  sim_normal_events();
  sim_double_hit_event();
}
