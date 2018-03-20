#ifndef EVENT_WINDOW_H
#define EVENT_WINDOW_H

#include <types.h>

#define KEY_MAX 256

#define EV_UNINIT_KEY  -1
#define EV_E_NONE       0
#define EV_E_OOB        1
#define EV_E_REM_UNINIT 2
#define EV_E_EXIST      3
#define EV_E_CONTIG     4
#define EV_E_EMPTY      5
#define EV_E_DNE        6

typedef struct event_window {
  int keys[KEY_MAX];
  int unexp_size;
  int size;
  int start;
  int end;
  bool new_window;
} event_window;

void ev_window_init(event_window *);

int ev_window_is_expected(event_window *, int key);

int ev_window_is_unexpected(event_window *, int key);

int ev_window_shift_all(event_window *);

int ev_window_expected_size(event_window *ew);

int ev_window_add_key(event_window *ew, int key);

int ev_window_remove_key(event_window *ew, int key);

int ev_window_add_keys(event_window *, int *, int);

int ev_window_inc_key(int key);
#endif
