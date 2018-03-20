#include <test/train/event_window.h>

static void ev_init() {
  event_window ev;
  ev_window_init(&ev);
  assert(ev.unexp_size == 0);
  assert(ev.size == 0);
}

static void ev_add_remove() {
  int r;
  event_window ev;
  ev_window_init(&ev);

  assert(!ev_window_is_unexpected(&ev, 0));
  assert(!ev_window_is_expected(&ev, 0));
  r = ev_window_add_key(&ev, 0);
  assert(r == 0);
  assert(ev.size == 1);
  assert(!ev_window_is_unexpected(&ev, 0));
  assert(ev_window_is_expected(&ev, 0));

  r = ev_window_add_key(&ev, 1);
  assert(r == 0);
  assert(ev.size == 2);
  assert(!ev_window_is_unexpected(&ev, 0));
  assert(ev_window_is_expected(&ev, 0));
  assert(!ev_window_is_unexpected(&ev, 1));
  assert(ev_window_is_expected(&ev, 1));

  r = ev_window_remove_key(&ev, 0);
  assert(r == 0);
  assert(ev.size == 1);
  assert(ev_window_is_unexpected(&ev, 0));
  assert(!ev_window_is_expected(&ev, 0));
  assert(!ev_window_is_unexpected(&ev, 1));
  assert(ev_window_is_expected(&ev, 1));

  r = ev_window_remove_key(&ev, 1);
  assert(r == 0);
  assert(ev.size == 0);
  assert(!ev_window_is_unexpected(&ev, 0));
  assert(!ev_window_is_expected(&ev, 0));
  assert(!ev_window_is_unexpected(&ev, 1));
  assert(!ev_window_is_expected(&ev, 1));

  r = ev_window_add_key(&ev, 3);
  assert(r == EV_E_CONTIG);
}

static void ev_wrap() {
  int r;
  event_window ev;
  ev_window_init(&ev);
  r = ev_window_add_key(&ev, KEY_MAX);
  assert(r == EV_E_OOB);
  ev_window_add_key(&ev, KEY_MAX-1);
  assert(!ev_window_is_unexpected(&ev, KEY_MAX-1));
  assert(ev_window_is_expected(&ev, KEY_MAX-1));
  ev_window_add_key(&ev, 0);
  assert(!ev_window_is_unexpected(&ev, KEY_MAX-1));
  assert(ev_window_is_expected(&ev, KEY_MAX-1));
  assert(!ev_window_is_unexpected(&ev, 0));
  assert(ev_window_is_expected(&ev, 0));
  assert(ev.size == 2);
  ev_window_remove_key(&ev, KEY_MAX-1);
  assert(ev.size == 1);
  ev_window_add_key(&ev, 1);
  assert(!ev_window_is_unexpected(&ev, 1));
  assert(ev_window_is_expected(&ev, 1));
}

static void ev_shift() {
  int r;
  event_window ev;
  ev_window_init(&ev);
  r = ev_window_add_key(&ev, 0);
  assert(r == 0);
  r = ev_window_add_key(&ev, 1);
  assert(r == 0);
  r = ev_window_add_key(&ev, 2);
  assert(r == 0);
  assert(ev.size == 3);

  assert(!ev_window_is_unexpected(&ev, 1));
  ev_window_remove_key(&ev, 1);
  assert(ev.size == 2);

  ev_window_shift_all(&ev);
  assert(ev.unexp_size == 2);

  r = ev_window_add_key(&ev, 8);
  assert(r == 0);
  r = ev_window_add_key(&ev, 9);
  assert(r == 0);
  r = ev_window_add_key(&ev, 10);
  assert(r == 0);
  assert(ev.size == 3);

  ev_window_remove_key(&ev, 0);
  ev_window_remove_key(&ev, 2);
  assert(ev.unexp_size == 0);
  assert(ev.size == 3);

  r = ev_window_remove_key(&ev, 8);
  assert(r == 0);
  r = ev_window_remove_key(&ev, 9);
  assert(r == 0);
  assert(ev.size == 1);

  ev_window_shift_all(&ev);
  assert(!ev_window_is_unexpected(&ev, 10));
  assert(!ev_window_is_expected(&ev, 10));
  r = ev_window_remove_key(&ev, 10);
  assert(r == 0);
  assert(ev.size == 0);
  assert(ev.unexp_size == 0);
}

static void ev_shift_wrap() {
  int r;
  event_window ev;
  ev_window_init(&ev);

  r = ev_window_add_key(&ev, KEY_MAX-5);
  r = ev_window_add_key(&ev, KEY_MAX-4);
  r = ev_window_add_key(&ev, KEY_MAX-3);

  r = ev_window_remove_key(&ev, KEY_MAX-3);
  assert(r == 0);
  r = ev_window_remove_key(&ev, KEY_MAX-4);
  assert(r == 0);
  assert(ev.size == 1);

  ev_window_shift_all(&ev);
  assert(ev.size == 0);
  assert(ev.unexp_size == 1);

  r = ev_window_add_key(&ev, 0);
  assert(r == 0);
  r = ev_window_add_key(&ev, 1);
  assert(r == 0);
  r = ev_window_add_key(&ev, 2);
  assert(r == 0);

  ev_window_shift_all(&ev);
  assert(ev.unexp_size == 4);
  assert(ev.size == 0);

}

static void ev_shift_contig() {
  int r;
  event_window ev;
  ev_window_init(&ev);

  ev_window_shift_all(&ev);
  r = ev_window_add_key(&ev, 0);
  assert(r == 0);
  r = ev_window_add_key(&ev, 1);
  assert(r == 0);
  r = ev_window_add_key(&ev, 2);
  assert(r == 0);

  ev_window_shift_all(&ev);
  r = ev_window_add_key(&ev, 4);
  assert(r == 0);
  r = ev_window_add_key(&ev, 5);
  assert(r == 0);
  r = ev_window_add_key(&ev, 6);
  assert(r == 0);

  ev_window_shift_all(&ev);
  r = ev_window_add_key(&ev, 7);
  assert(r == 0);

  ev_window_shift_all(&ev);
  r = ev_window_add_key(&ev, 9);
  assert(ev.size == 1);
  assert(ev.unexp_size == 7);
  assert(r == 0);
}

static void ev_shift_wrap2() {
  int r;
  event_window ev;
  ev_window_init(&ev);

  r = ev_window_add_key(&ev, KEY_MAX-5);
  assert(r == 0);
  r = ev_window_add_key(&ev, KEY_MAX-4);
  assert(r == 0);
  r = ev_window_add_key(&ev, KEY_MAX-3);
  assert(r == 0);
  r = ev_window_add_key(&ev, KEY_MAX-2);
  assert(r == 0);
  r = ev_window_add_key(&ev, KEY_MAX-1);
  assert(r == 0);
  r = ev_window_add_key(&ev, 0);
  assert(r == 0);
}


void event_window_tests() {
  ev_init();
  ev_add_remove();
  ev_shift();
  ev_wrap();
  ev_shift_wrap();
  ev_shift_wrap2();
  ev_shift_contig();
}
