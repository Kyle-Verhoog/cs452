#include <test/train/reservation.h>

static track_node T[TRACK_MAX];

void pather_init_test() {
  pather p;
  pather_init(&p, T);
}

void get_nodes_in_dist() {
  tn_q ret;
  track_node *s;
  int max_dist;

  s = &T[trhr(T, "A1")];
  max_dist = 231+188;

  get_all_nodes_in_dist(s, max_dist, &ret);
  assert(ret.size == 3);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "A1"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "MR12"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "MR11"));

  s = &T[trhr(T, "A1")];
  max_dist = 231+188-1;
  get_all_nodes_in_dist(s, max_dist, &ret);
  assert(ret.size == 2);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "A1"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "MR12"));

  s = &T[trhr(T, "C11")];
  max_dist = 600;
  get_all_nodes_in_dist(s, max_dist, &ret);
  assert(ret.size == 5);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "C11"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "BR13"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "E16"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "B5"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "E1"));

  s = &T[trhr(T, "C11")];
  max_dist = 770;
  get_all_nodes_in_dist(s, max_dist, &ret);
  assert(ret.size == 6);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "C11"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "BR13"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "E16"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "B5"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "E1"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "D3"));

  s = &T[trhr(T, "C11")];
  max_dist = 769;
  get_all_nodes_in_dist(s, max_dist, &ret);
  assert(ret.size == 6);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "C11"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "BR13"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "E16"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "B5"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "E1"));

  s = &T[trhr(T, "A1")];
  max_dist = 0;
  get_all_nodes_in_dist(s, max_dist, &ret);
  assert(ret.size == 1);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "A1"));
}

void get_sens_in_dist() {
  tn_q ret;
  track_node *s;
  int dist;

  s = &T[trhr(T, "C11")];
  dist = 121;

  get_all_to_sen_in_dist(s, dist, &ret);
  assert(ret.size == 2);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "C11"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "BR13"));
  tn_q_pop(&ret, &s);


  s = &T[trhr(T, "C11")];
  dist = 350;

  get_all_to_sen_in_dist(s, dist, &ret);
  assert(ret.size == 2);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "C11"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "BR13"));
  tn_q_pop(&ret, &s);


  s = &T[trhr(T, "C11")];
  dist = 351;

  get_all_to_sen_in_dist(s, dist, &ret);
  assert(ret.size == 3);
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "C11"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "BR13"));
  tn_q_pop(&ret, &s);
  assert(streq(s->name, "B5"));
  // s = &T[trhr(T, "A1")];
  // max_dist = 231+188-1;
  // get_all_nodes_in_dist(s, max_dist, &ret);
  // assert(ret.size == 2);
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "A1"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "MR12"));

  // s = &T[trhr(T, "C11")];
  // max_dist = 600;
  // get_all_nodes_in_dist(s, max_dist, &ret);
  // assert(ret.size == 5);
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "C11"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "BR13"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "E16"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "B5"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "E1"));

  // s = &T[trhr(T, "C11")];
  // max_dist = 770;
  // get_all_nodes_in_dist(s, max_dist, &ret);
  // assert(ret.size == 6);
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "C11"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "BR13"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "E16"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "B5"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "E1"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "D3"));

  // s = &T[trhr(T, "C11")];
  // max_dist = 769;
  // get_all_nodes_in_dist(s, max_dist, &ret);
  // assert(ret.size == 6);
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "C11"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "BR13"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "E16"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "B5"));
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "E1"));

  // s = &T[trhr(T, "A1")];
  // max_dist = 0;
  // get_all_nodes_in_dist(s, max_dist, &ret);
  // assert(ret.size == 1);
  // tn_q_pop(&ret, &s);
  // assert(streq(s->name, "A1"));
}



void pather_reserve_test() {
  pather p;
  track_node *tn;
  int stop_dist;
  pather_init(&p, T);

  tn = &T[trhr(T, "C11")];
  stop_dist = 119;
  pather_reserve(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 1);

  pather_init(&p, T);
  tn = &T[trhr(T, "C11")];
  stop_dist = 120;
  pather_reserve(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 2);


  // test reserving C1, E16, E1
  pather_init(&p, T);
  tn = &T[trhr(T, "C11")];
  stop_dist = 120;
  pather_reserve(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 2);

  tn = &T[trhr(T, "E16")];
  stop_dist = 120;
  pather_reserve(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 3);

  tn = &T[trhr(T, "E1")];
  stop_dist = 120;
  pather_reserve(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 4);

  tn = &T[trhr(T, "E1")];
  stop_dist = 120;
  pather_reserve(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 4);
}

void pather_reserve_to_sensor_test() {
  pather p;
  track_node *tn;
  int stop_dist;
  pather_init(&p, T);

  tn = &T[trhr(T, "C11")];
  stop_dist = 119;
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 2);
  assert(p.reserv[trhr(T, "C11")].reserved);
  assert(p.reserv[trhr(T, "BR13")].reserved);
  assert(p.reserv[trhr(T, "MR13")].reserved);
  assert(p.reserv[trhr(T, "B6")].reserved);
  assert(p.reserv[trhr(T, "E15")].reserved);
  assert(!p.reserv[trhr(T, "C12")].reserved);
  assert(!p.reserv[trhr(T, "B5")].reserved);
  assert(!p.reserv[trhr(T, "E16")].reserved);

  tn = &T[trhr(T, "B5")];
  stop_dist = 119;
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 2+1);
  assert(p.reserv[trhr(T, "C11")].reserved);
  assert(p.reserv[trhr(T, "BR13")].reserved);
  assert(p.reserv[trhr(T, "MR13")].reserved);
  assert(p.reserv[trhr(T, "B6")].reserved);
  assert(p.reserv[trhr(T, "E15")].reserved);
  assert(!p.reserv[trhr(T, "C12")].reserved);
  assert(!p.reserv[trhr(T, "E16")].reserved);
  assert(p.reserv[trhr(T, "B5")].reserved);
  assert(p.reserv[trhr(T, "D4")].reserved);


  // test entering a merge
  pather_init(&p, T);
  tn = &T[trhr(T, "B6")];
  stop_dist = 119;
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 2);
  assert(p.reserv[trhr(T, "C11")].reserved);
  assert(p.reserv[trhr(T, "BR13")].reserved);
  assert(p.reserv[trhr(T, "MR13")].reserved);
  assert(p.reserv[trhr(T, "B6")].reserved);
  assert(p.reserv[trhr(T, "E15")].reserved);
  assert(!p.reserv[trhr(T, "C12")].reserved);
  assert(!p.reserv[trhr(T, "B5")].reserved);
  assert(!p.reserv[trhr(T, "E16")].reserved);
  assert(!p.reserv[trhr(T, "D3")].reserved);

  pather_reserve_to_sensor(&p, 24, tn, stop_dist);

  // pather_init(&p, T);
  // tn = &T[trhr(T, "C11")];
  // stop_dist = 120;
  // pather_reserve_to_sensor(&p, 24, tn, stop_dist);
  // assert(p.nreservs[24] == 2);
}

void pather_free_test_1() {
  pather p;
  track_node *tn;
  int stop_dist;
  pather_init(&p, T);

  tn = &T[trhr(T, "C11")];
  stop_dist = 119;
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);
  assert(p.nreservs[24] == 2);

  tn = &T[trhr(T, "E16")];
  stop_dist = 119;
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);

  tn = &T[trhr(T, "E1")];
  stop_dist = 119;
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);

  assert(p.reserv[trhr(T, "C11")].reserved);
  assert(p.reserv[trhr(T, "BR13")].reserved);
  assert(p.reserv[trhr(T, "MR13")].reserved);
  assert(p.reserv[trhr(T, "B6")].reserved);
  assert(p.reserv[trhr(T, "E15")].reserved);
  assert(p.reserv[trhr(T, "E16")].reserved);
  assert(p.reserv[trhr(T, "BR156")].reserved);
  assert(p.reserv[trhr(T, "E1")].reserved);
  assert(p.reserv[trhr(T, "E2")].reserved);

  pather_free_before(&p, 24, &T[trhr(T, "E16")]);
  assert(!p.reserv[trhr(T, "C11")].reserved);
  assert(!p.reserv[trhr(T, "MR13")].reserved);
  assert(!p.reserv[trhr(T, "BR13")].reserved);
  assert(!p.reserv[trhr(T, "E15")].reserved);
  assert(!p.reserv[trhr(T, "B6")].reserved);
}

void pather_free_test_2() {
  pather p;
  track_node *tn;
  int stop_dist;
  pather_init(&p, T);

  tn = &T[trhr(T, "C11")];
  stop_dist = 119;
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);
  tn = &T[trhr(T, "E16")];
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);
  tn = &T[trhr(T, "E1")];
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);
  tn = &T[trhr(T, "C1")];
  pather_reserve_to_sensor(&p, 24, tn, stop_dist);

  assert(p.reserv[trhr(T, "C11")].reserved);
  assert(p.reserv[trhr(T, "BR13")].reserved);
  assert(p.reserv[trhr(T, "MR13")].reserved);
  assert(p.reserv[trhr(T, "B6")].reserved);
  assert(p.reserv[trhr(T, "E15")].reserved);
  assert(p.reserv[trhr(T, "E16")].reserved);
  assert(p.reserv[trhr(T, "BR156")].reserved);
  assert(p.reserv[trhr(T, "E1")].reserved);
  assert(p.reserv[trhr(T, "E2")].reserved);
  assert(p.reserv[trhr(T, "MR156")].reserved);
  assert(p.reserv[trhr(T, "BR156")].reserved);
  assert(p.reserv[trhr(T, "BR154")].reserved);
  assert(p.reserv[trhr(T, "MR154")].reserved);
  assert(p.reserv[trhr(T, "BR153")].reserved);
  assert(p.reserv[trhr(T, "B13")].reserved);
  assert(p.reserv[trhr(T, "MR153")].reserved);
  assert(p.reserv[trhr(T, "EN1")].reserved);

  pather_free_before(&p, 24, &T[trhr(T, "C1")]);
  assert(!p.reserv[trhr(T, "C11")].reserved);
  assert(!p.reserv[trhr(T, "BR13")].reserved);
  assert(!p.reserv[trhr(T, "MR13")].reserved);
  assert(!p.reserv[trhr(T, "B6")].reserved);
  assert(!p.reserv[trhr(T, "E15")].reserved);
  assert(!p.reserv[trhr(T, "E16")].reserved);
  assert(!p.reserv[trhr(T, "BR156")].reserved);
  assert(!p.reserv[trhr(T, "E1")].reserved);
  assert(!p.reserv[trhr(T, "E2")].reserved);
  assert(!p.reserv[trhr(T, "MR156")].reserved);
  assert(!p.reserv[trhr(T, "BR156")].reserved);
  assert(!p.reserv[trhr(T, "BR154")].reserved);
  assert(!p.reserv[trhr(T, "MR154")].reserved);
  assert(!p.reserv[trhr(T, "BR153")].reserved);
  assert(!p.reserv[trhr(T, "B13")].reserved);
  assert(!p.reserv[trhr(T, "MR153")].reserved);
  assert(!p.reserv[trhr(T, "EN1")].reserved);
}


void reservation_tests() {
  init_tracka(T);
  get_nodes_in_dist();
  pather_init_test();
  get_sens_in_dist();
  pather_reserve_test();
  pather_free_test_1();
  pather_free_test_2();
  pather_reserve_to_sensor_test();
}
