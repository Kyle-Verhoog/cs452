#ifndef EVENT_WINDOW_H
#define EVENT_WINDOW_H

#include <types.h>
#include <defines.h>
#include <lib/circular_buffer.h>
#include <lib/train/track_node.h>
#include <lib/train/train_defines.h>
#include <lib/train/track_data.h>

#define KEY_MAX 256

#define EV_UNINIT_KEY  -1
#define EV_WIND_TO      1
#define EV_WIND_MULTI   2

#define WINDOW_SIZE 2


typedef enum event_res_t {
  UNDEF,
  HIT,
  TIMEOUT,
  EVENT_MAX,
} event_res_t;

typedef struct event_window {
  int key_offset;
  track_node *node;
  int nevents;
  int num_event[EVENT_MAX];
} event_window;

EXT_CIRCULAR_BUFFER_DEC(ev_w_q, event_window *, TRACK_MAX);

typedef struct ev_window_manager {
  event_window window[TRACK_MAX];
  ev_w_q avail_windows;
  ev_w_q window_q;
  event_window *window_map[KEY_MAX];
} ev_wm;


void ev_wm_init(ev_wm *);

int ev_wm_add_to_window(ev_wm *wm, int key, track_node *node);

int ev_wm_next_window(ev_wm *wm);

int ev_wm_res_to_window(ev_wm *wm, int key, int res);

track_node *ev_wm_get_window_tn(ev_wm *wm, int key);

int ev_wm_invalidate_after(ev_wm *wm, int key);

int ev_wm_delete_if_complete(ev_wm *wm, int key);

int ev_wm_next_key(int key);

int ev_wm_delete_all(ev_wm *wm);

#endif
