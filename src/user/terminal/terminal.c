#include <user/terminal/terminal.h>

CIRCULAR_BUFFER_DEF(tdisp_cb, char, TERMINAL_BUFFER_SIZE);
CIRCULAR_BUFFER_DEF(wid_cb, int, MAX_WINDOWS);

int tdisp_cb_pushui32(tdisp_cb *buf, unsigned int i) {
  int n = 0;
  int dgt, offset;
  char c;
  unsigned int d;

  d = 1;
  offset = 0;

  while ((i / d) >= 10) d *= 10;
  while (d != 0) {
    dgt = i / d;
    i %= d;
    d /= 10;
    if (n || dgt > 0 || d == 0) {
      c = dgt + (dgt < 10 ? '0' : 'a' - 10);
      tdisp_cb_push(buf, c);
      ++n;
    }
  }
  return offset;
}

void tcursor_init(TCursor *tc, int x, int y) {
  tc->x = x;
  tc->y = y;
}

void twindow_init(TWindow *tw, int x, int y, int w, int h) {
  tcursor_init(&tw->cur, 1, 1);
  tw->wid = -1;
  tw->offsetx = x;
  tw->offsety = y;
  tw->w = w;
  tw->h = h;
}

void tdisp_init(TDisplay *td) {
  int i;
  td->num_active_windows = 0;
  tcursor_init(&td->tdcur, -1, -1);
  tdisp_cb_init(&td->buffer);
  wid_cb_init(&td->avail_wids);

  for (i = 0; i < MAX_WINDOWS; ++i) {
    td->active_windows[i] = 0;
    wid_cb_push(&td->avail_wids, i);
  }
}

char tdisp_pop(TDisplay *td) {
  int r;
  char c;

  r = tdisp_cb_pop(&td->buffer, &c);
  // assert(r == 0);
  return c;
}

// set cursor relative to window
void tdisp_set_cursor(TDisplay *td, int x, int y) {
  TWindow *window;
  int ret;
  tdisp_cb *buf;
  buf = &td->buffer;
  window = td->focused_window;

  x += window->offsetx;
  y += window->offsety;

  if (td->tdcur.x != x || td->tdcur.y != y) {
    ret = tdisp_cb_push(buf, '\033');
    ret = tdisp_cb_push(buf, '[');
    ret = tdisp_cb_pushui32(buf, y);
    ret = tdisp_cb_push(buf, ';');
    ret = tdisp_cb_pushui32(buf, x);
    ret = tdisp_cb_push(buf, 'H');
  }

  td->tdcur.x = x;
  td->tdcur.y = y;
}

// resets the cursor to the focused window's cursor position
void tdisp_reset_cursor(TDisplay *td) {
  TWindow *window;
  window = td->focused_window;
  if (window->cur.x != td->tdcur.x || window->cur.y != td->tdcur.y) {
    tdisp_set_cursor(td, window->cur.x, window->cur.y);
  }
}

void tdisp_set_active_window(TDisplay *td, int wid) {
  td->focused_window = &td->windows[wid];
}

void tdisp_draw_window_outline(TDisplay *td) {
  int i, j, x, y, w, h;
  TWindow *window;
  window = td->focused_window;

  x = window->offsetx;
  y = window->offsety;
  w = window->w;
  h = window->h;

  for (i = 0; i < h; ++i) {
    if (i == 0 || i == h-1) {
      tdisp_set_cursor(td, 0, i);
      tdisp_cb_push(&td->buffer, '+');
      for (j = 0; j < w-1; ++j)
        tdisp_cb_push(&td->buffer, '-');
      tdisp_set_cursor(td, w-1, i);
      tdisp_cb_push(&td->buffer, '+');
    }
    else {
      tdisp_set_cursor(td, 0, i);
      tdisp_cb_push(&td->buffer, '|');
      tdisp_set_cursor(td, w-1, i);
      tdisp_cb_push(&td->buffer, '|');
    }
  }
}

void tdisp_clear_window(TDisplay *td) {
  int i, j, w, h;
  TWindow *window;
  window = td->focused_window;

  w = window->w;
  h = window->h;

  // tdisp_set_cursor(td, window->offsetx, window->offsety);
  for (i = 0; i < h; ++i) {
    tdisp_set_cursor(td, 0, i);
    for (j = 0; j < w; ++j) {
      tdisp_cb_push(&td->buffer, ' ');
    }
  }
}


void tdisp_add_window(TDisplay *td, int x, int y, int w, int h) {
  int r, wid;
  TWindow *window;
  // assert(td->avail_wids.size > 0);

  r = wid_cb_pop(&td->avail_wids, &wid);
  // assert(r == 0);
  td->active_windows[wid] = 1;
  window = &td->windows[wid];
  twindow_init(window, x, y, w, h);
  tdisp_set_active_window(td, wid);
  // assert(wid >= 0 && wid <= MAX_WINDOWS);
  tdisp_draw_window_outline(td);
  tdisp_reset_cursor(td);
}

void tdisp_focus_window(TDisplay *td, int wid) {
  // assert(td->active_windows[wid]);
  TWindow *window;
  window = &td->windows[wid];
  td->focused_window = window;
  tdisp_reset_cursor(td);
}

void tdisp_delete_window(TDisplay *td) {
  TWindow *fwindow;
  int i, r;
  int wid;
  fwindow = td->focused_window;
  wid = fwindow->wid;

  tdisp_clear_window(td);
  td->active_windows[wid] = 0;
  r = wid_cb_push(&td->avail_wids, wid);
  td->focused_window = 0;

  // focus another window if it exists
  for (i = 0; i < MAX_WINDOWS; ++i) {
    if (td->active_windows[i]) {
      tdisp_focus_window(td, i);
      break;
    }
  }
}


// writes a character to the active window
void tdisp_writec(TDisplay *td, char c) {
  tdisp_reset_cursor(td);
  tdisp_cb_push(&td->buffer, c);
  td->tdcur.x++;
  td->focused_window->cur.x++;
}
