#include <test/train/event_window.h>

static track_node track[TRACK_MAX];

static void ev_wm_init_test() {
  ev_wm wm;
  ev_wm_init(&wm);
}

static void ev_wm_add_to_window_test() {
  int r;
  ev_wm wm;
  track_node *tn;
  ev_wm_init(&wm);

  tn = &track[trhr(track, "A1")];
  r = ev_wm_add_to_window(&wm, 0, tn);
  assert(r == 0);

  assert(wm.window_map[0] == &wm.window[0]);

  r = ev_wm_next_window(&wm);
  assert(r == 0);

  r = ev_wm_add_to_window(&wm, 2, tn);
  assert(r == 0);
  assert(wm.window_map[2] == &wm.window[1]);


  ev_wm_init(&wm);
  r = ev_wm_add_to_window(&wm, 0, tn);
  assert(r == 0);
  r = ev_wm_add_to_window(&wm, 1, tn);
  assert(r == 0);

  assert(wm.window_map[0] == &wm.window[0]);
  assert(wm.window_map[1] == &wm.window[0]);

  r = ev_wm_next_window(&wm);
  assert(r == 0);

  r = ev_wm_add_to_window(&wm, 2, tn);
  assert(r == 0);
  r = ev_wm_add_to_window(&wm, 3, tn);
  assert(r == 0);
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


  // train 1 starts and events are generated for A1
  tn = &track[trhr(track, "A1")];
  ev_wm_add_to_window(&wm, 0, tn);
  ev_wm_next_window(&wm);

  // train 1 hits A1
  r = ev_wm_res_to_window(&wm, 0, HIT);
  assert(r == 3);
  r = ev_wm_delete_if_complete(&wm, 0);
  assert(r == 0);


  // train 1 is expected to hit C13
  tn = &track[trhr(track, "C13")];
  r = ev_wm_add_to_window(&wm, 1, tn);
  assert(r == 0);
  r = ev_wm_next_window(&wm);
  assert(r == 0);

  // train 1 hits C13
  r = ev_wm_res_to_window(&wm, 1, HIT);
  assert(r == 3);
  r = ev_wm_delete_if_complete(&wm, 1);
  assert(r == 0);


  // train 1 is expected to hit E7
  tn = &track[trhr(track, "E7")];
  r = ev_wm_add_to_window(&wm, 2, tn);
  assert(r == 0);
  r = ev_wm_next_window(&wm);
  assert(r == 0);

  // train 1 didn't reach E7 in time
  r = ev_wm_res_to_window(&wm, 2, TIMEOUT);
  assert(r == 1);

  r = ev_wm_invalidate_after(&wm, 2);
  assert(r == 0);
  // printf("%d\n", wm.window_map[2]->nevents);
  // printf("%d %d\n", wm.window_map[3], wm.window_map[2]);


  /*
  tn = &track[trhr(track, "C1")];
  ev_wm_add_to_window(&wm, 3, tn);
  ev_wm_add_to_window(&wm, 4, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_res_to_window(&wm, 3, TIMEOUT);
  assert(r == 0);
  r = ev_wm_res_to_window(&wm, 4, TIMEOUT);
  assert(r == 1);
  */

  /*
  tn = NULL;
  tn = ev_wm_get_window_tn(&wm, 4);
  assert(tn == &track[trhr(track, "C1")]);

  tn = &track[trhr(track, "E1")];
  ev_wm_add_to_window(&wm, 5, tn);
  ev_wm_add_to_window(&wm, 6, tn);
  ev_wm_next_window(&wm);

  tn = NULL;
  tn = ev_wm_get_window_tn(&wm, 5);
  assert(tn == &track[trhr(track, "E1")]);
  tn = NULL;
  tn = ev_wm_get_window_tn(&wm, 6);
  assert(tn == &track[trhr(track, "E1")]);

  r = ev_wm_res_to_window(&wm, 5, HIT);
  assert(r == 0);

  tn = &track[trhr(track, "E5")];
  ev_wm_add_to_window(&wm, 7, tn);
  ev_wm_next_window(&wm);

  r = ev_wm_res_to_window(&wm, 6, HIT);
  assert(r == 2);
  tn = NULL;
  tn = ev_wm_get_window_tn(&wm, 6);
  assert(tn == &track[trhr(track, "E1")]);
  */
}

static void ev_wm_invalidate_test() {
  ev_wm wm;
  int r;
  track_node *tn;
  ev_wm_init(&wm);

  tn = &track[trhr(track, "A1")];
  ev_wm_add_to_window(&wm, 0, tn);
  ev_wm_add_to_window(&wm, 1, tn);
  ev_wm_next_window(&wm);

  tn = &track[trhr(track, "C13")];
  ev_wm_add_to_window(&wm, 2, tn);
  ev_wm_next_window(&wm);
  r = ev_wm_res_to_window(&wm, 0, HIT);
  assert(r == 0);


  tn = &track[trhr(track, "D9")];
  ev_wm_add_to_window(&wm, 3, tn);
  ev_wm_next_window(&wm);

  // double hit
  r = ev_wm_res_to_window(&wm, 1, HIT);
  assert(r == 2);

  tn = ev_wm_get_window_tn(&wm, 1);
  assert(tn == &track[trhr(track, "A1")]);

  ev_wm_invalidate_after(&wm, 1);

  // rest of events should be null'd
  r = ev_wm_res_to_window(&wm, 2, HIT);
  assert(r == -1);
  r = ev_wm_res_to_window(&wm, 3, HIT);
  assert(r == -1);
}


static void ev_stress() {
  ev_wm wm;
  int i, r;
  track_node *tn;
  ev_wm_init(&wm);

  for (i = 0; i < 10/*KEY_MAX*10*/; i += 2) {
    tn = &track[trhr(track, "A1")];
    ev_wm_add_to_window(&wm, i%KEY_MAX, tn);
    // printf("%d: %d %d %d\n", i%KEY_MAX, r, wm.window_map[i%KEY_MAX]->num_event[HIT], wm.window_map[i%KEY_MAX]->num_event[TIMEOUT]);
    ev_wm_add_to_window(&wm, (i+1)%KEY_MAX, tn);
    r = ev_wm_next_window(&wm);
    assert(r == 0);
    // printf("%d: %d %d %d\n", i%KEY_MAX, r, wm.window_map[i%KEY_MAX]->num_event[HIT], wm.window_map[i%KEY_MAX]->num_event[TIMEOUT]);
    r = ev_wm_res_to_window(&wm, i%KEY_MAX, HIT);
    //assert(wm.window_map[i%KEY_MAX]->num_event[HIT]
    // printf("%d: %d %d %d\n", i%KEY_MAX, r, wm.window_map[i%KEY_MAX]->num_event[HIT], wm.window_map[i%KEY_MAX]->num_event[TIMEOUT]);
    assert(r == 0);
    r = ev_wm_res_to_window(&wm, (i+1)%KEY_MAX, TIMEOUT);
    assert(r == 3);

    r = ev_wm_delete_if_complete(&wm, i%KEY_MAX);
    // assert (r == 0);

  }
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
  ev_wm_invalidate_test();
  sim_normal_events();
  sim_double_hit_event();
  ev_stress();
}
