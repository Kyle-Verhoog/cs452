#ifndef EVENT_WINDOW_H
#define EVENT_WINDOW_H

#include <types.h>

#define KEY_MAX 256

#define EV_UNINIT_KEY -1
#define EV_E_NONE 0

typedef struct event_window {
  int keys[KEY_MAX];
  int unexp_start;
  int unexp_end;
  int exp_start;
  int exp_end;
} event_window;

void ev_window_init(event_window *);

int ev_window_is_unexpected(event_window *, int key);

int ev_window_shift_all(event_window *);

int ev_window_add_key(event_window *ew, int key);

int ev_window_add_keys(event_window *, int *, int);


#endif
