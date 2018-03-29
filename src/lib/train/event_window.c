#include <lib/train/event_window.h>

EXT_CIRCULAR_BUFFER_DEF(ev_w_q, event_window *, TRACK_MAX);

void event_window_init(event_window *evw) {
  int i;
  evw->key_offset = -1;
  evw->node = NULL;
  evw->nevents = 0;

  for (i = 0; i < EVENT_MAX; ++i) {
    evw->num_event[i] = 0;
  }
}


void ev_wm_init(ev_wm *wm) {
  int i, r;
  event_window *window;
  ev_w_q_init(&wm->window_q);
  ev_w_q_init(&wm->avail_windows);

  for (i = 0; i < TRACK_MAX; ++i) {
    window = &wm->window[i];
    event_window_init(window);
    r = ev_w_q_push(&wm->avail_windows, window);
#ifndef X86    
    assert(r != CB_E_FULL);
#endif
  }

  for (i = 0; i < KEY_MAX; ++i) {
    wm->window_map[i] = NULL;
  }
}


// adds to the current window
// returns
//    0 on success
//   -1 if the key is associated with another window already
int ev_wm_add_to_window(ev_wm *wm, int key, track_node *node) {
  int i;
  event_window *cur_window;
#ifndef X86
  assert(key >= 0 && key < KEY_MAX);
#endif
  if (wm->window_map[key]) {
    return -1;
  }

  ev_w_q_get(&wm->avail_windows, 0, &cur_window);

  // init the window if it hasn't been yet
  if (!cur_window->node) {
    cur_window->key_offset = key;
    cur_window->node = node;
    cur_window->nevents = 0;
    for (i = 0; i < EVENT_MAX; ++i)
      cur_window->num_event[i] = 0;
  }

  cur_window->nevents++;

  // assert(cur_window->nevents < EVENT_MAX);

  // add the mapping
  wm->window_map[key] = cur_window;

  return 0;
}


// starts a new window
int ev_wm_next_window(ev_wm *wm) {
  int r;
  event_window *cur_window;
  r = ev_w_q_pop(&wm->avail_windows, &cur_window);
  if (r) return r;
  r = ev_w_q_push(&wm->window_q, cur_window);
  return r;
}


int ev_wm_next_key(int key) {
  return (key+1) % KEY_MAX;
}


// add a result to the window
// returns
//  - -1 if the window does not exist
//  -  0 if adding the result has no effect
//  -  1 if all results in the window are VRE VE (the whole window timed out)
//  -  2 if there are conflicting VRE VE/RE results
//  -  3 if the window has been filled and needs to be cleared
int ev_wm_res_to_window(ev_wm *wm, int key, int res) {
  event_window *window;
#ifndef X86  
  assert(key >= 0 && key < KEY_MAX);
  assert(res >= 0 && res < EVENT_MAX);
#endif

  window = wm->window_map[key];
  if (!window) return -1;

  window->num_event[res]++;

  if (window->nevents == window->num_event[TIMEOUT]) {
    return 1;
  }
  if (window->num_event[HIT] > 1) {
    return 2;
  }
  if (window->nevents == window->num_event[TIMEOUT] + window->num_event[HIT]) {
    return 3;
  }

  return 0;
}


int ev_wm_delete_if_complete(ev_wm *wm, int key) {
  int i, k, r;
  event_window *window;
#ifndef X86
  assert(key >= 0 && key < KEY_MAX);
#endif

  window = wm->window_map[key];

  if (!window)
    return -1;

  if (!(window->nevents == window->num_event[TIMEOUT] + window->num_event[HIT]))
    return -2;

  r = ev_w_q_rem(&wm->window_q, window);
  if (r)
    return -3;

  // clear the map entries
  for (i = 0; i < window->nevents; ++i) {
    k = (i + window->key_offset)%KEY_MAX;
#ifndef X86
    assert(k >= 0 && k < KEY_MAX);
#endif
    wm->window_map[k] = NULL;
  }

  event_window_init(window);

  r = ev_w_q_push(&wm->avail_windows, window);
  if (r) return -4;

  return 0;
}


track_node *ev_wm_get_window_tn(ev_wm *wm, int key) {
  event_window *window;
#ifndef X86
  assert(key >= 0 && key < KEY_MAX);
#endif
  window = wm->window_map[key];
  return window->node;
}


int ev_wm_invalidate_after(ev_wm *wm, int key) {
  int k, r;
  event_window *end, *window;
#ifndef X86
  assert(key >= 0 && key < KEY_MAX);
#endif

  window = NULL;
  end = wm->window_map[key];

  while (wm->window_q.size > 0 && window != end) {
    r = ev_w_q_pop_end(&wm->window_q, &window);
    assert(r == 0);
    for (k = window->key_offset%KEY_MAX; k < window->key_offset + window->nevents; k = (k+1)%KEY_MAX) {
#ifndef X86
      assert(k >= 0 && k < KEY_MAX);
#endif      
      wm->window_map[k] = NULL;
    }
    window->node = NULL;
    window->key_offset = -1;
    r = ev_w_q_push(&wm->avail_windows, window);
#ifndef X86
    assert(r != CB_E_FULL);
#endif
  }

  r = ev_w_q_push(&wm->window_q, end);
#ifndef X86
  assert(r != CB_E_FULL);
#endif
  return 0;
}
