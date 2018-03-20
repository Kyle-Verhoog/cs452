#include <lib/train/event_window.h>

void ev_window_init(event_window *ew) {
  int i;
  for (i = 0; i < KEY_MAX; ++i) {
    ew->keys[i] = 0;
  }

  ew->start = 0;
  ew->end = 0;
  ew->size = 0;
  ew->unexp_size = 0;
  ew->new_window = true;
}

int ev_window_is_expected(event_window *ew, int key) {
  if (ew->size == 0 || key < 0 || key >= KEY_MAX) {
    return false;
  }

  if (ew->start < ew->end && ew->start <= key && key < ew->end) {
    return ew->keys[key];
  }
  else if (ew->start > ew->end && (key >= ew->start || key < ew->end)) {
    return ew->keys[key];
  }
  else {
    return false;
  }
}


bool ev_window_is_unexpected(event_window *ew, int key) {
  if (ew->size == 0 || key < 0 || key >= KEY_MAX) {
    return false;
  }

  if (ew->start < ew->end && ew->start <= key && key < ew->end) {
    return !ew->keys[key];
  }
  else if (ew->start > ew->end && (key >= ew->start || key < ew->end)) {
    return !ew->keys[key];
  }
  else {
    return ew->keys[key];
  }
}

int ev_window_remove_key(event_window *ew, int key) {
  if (key < 0 || key >= KEY_MAX)
    return EV_E_OOB;

  if (!ew->keys[key]) {
    return EV_E_REM_UNINIT;
  }

  if (ew->start < ew->end && ew->start <= key && key < ew->end) {
    if (ew->size == 0)
      return EV_E_EMPTY;
    ew->keys[key] = 0;
    ew->size--;
  }
  else if (ew->start > ew->end && (key >= ew->start || key < ew->end)) {
    if (ew->size == 0)
      return EV_E_EMPTY;
    ew->keys[key] = 0;
    ew->size--;
  }
  else {
    ew->keys[key] = 0;
    ew->unexp_size--;
  }

  return EV_E_NONE;
}

int ev_window_shift_all(event_window *ew) {
  ew->unexp_size += ew->size;
  ew->size = 0;
  ew->start = ew->end;
  ew->new_window = true;
  return 0;
}

int ev_window_add_key(event_window *ew, int key) {
  if (key < 0 || key >= KEY_MAX)
    return EV_E_OOB;

  if (ew->keys[key])
    return EV_E_EXIST;

  if (ew->new_window) {
    ew->keys[key] = 1;
    ew->start = key;
    ew->end = (key+1) % KEY_MAX;
    ew->size++;
    ew->new_window = false;
    return EV_E_NONE;
  }
  else if (key != ew->end) {
    return EV_E_CONTIG;
  }
  else {
    ew->keys[key] = 1;
    ew->end = (ew->end + 1) % KEY_MAX;
    ew->size++;
    return EV_E_NONE;
  }
  return EV_E_NONE;
}

int ev_window_inc_key(int key) {
  return (key+1) % KEY_MAX;
}

