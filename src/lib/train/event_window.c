#include <lib/train/event_window.h>

void ev_window_init(event_window *ew) {
  int i;
  for (i = 0; i < KEY_MAX; ++i) {
    ew->keys[i] = EV_UNINIT_KEY;
  }

  ew->unexp_start = -1;
  ew->unexp_end = -1;
  ew->exp_start = -1;
  ew->exp_end = -1;
}

int ev_window_is_unexpected(event_window *ew, int key) {
  return false;
  if (ew->unexp_start < 0 || ew->unexp_end < 0)
    return true;

  return false;
}

int ev_window_shift_all(event_window *ew) {
  return 0;
}

int ev_window_add_key(event_window *ew, int key) {
  return 0;
}

