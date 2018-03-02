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
  tcursor_init(&tw->cur, x + 1, y + 1);
  tw->x = x;
  tw->y = y;
  tw->w = w;
  tw->h = h;
}

void tdisp_init(TDisplay *td) {
  int i;
  td->num_active_windows = 0;
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

void tdisp_set_cursor(TDisplay *td, int x, int y) {
  TWindow *window;
  int ret;
  tdisp_cb *buf;
  buf = &td->buffer;
  window = td->focused_window;
  window->cur.x = x;
  window->cur.y = y;

  ret = tdisp_cb_push(buf, '\033');
  ret = tdisp_cb_push(buf, '[');
  ret = tdisp_cb_pushui32(buf, y);
  ret = tdisp_cb_push(buf, ';');
  ret = tdisp_cb_pushui32(buf, x);
  ret = tdisp_cb_push(buf, 'H');
}

void tdisp_set_active_window(TDisplay *td, int wid) {
  td->focused_window = &td->windows[wid];
}

void tdisp_draw_window_outline(TDisplay *td, int wid) {
  int i, j, x, y, w, h;
  TWindow *window;
  window = &td->windows[wid];
  x = window->x;
  y = window->y;
  w = window->w;
  h = window->h;

  for (i = y; i < y + h; ++i) {
    if (i == y || i == y + h - 1) {
      tdisp_set_cursor(td, x, i);
      tdisp_cb_push(&td->buffer, '+');
      for (j = x; j < x + h; ++j)
        tdisp_cb_push(&td->buffer, '-');
      tdisp_set_cursor(td, x + w, i);
      tdisp_cb_push(&td->buffer, '+');
    }
    else {
      tdisp_set_cursor(td, x, i);
      tdisp_cb_push(&td->buffer, '|');
      tdisp_set_cursor(td, x + w, i);
      tdisp_cb_push(&td->buffer, '|');
    }
  }
}

void tdisp_add_window(TDisplay *td, int x, int y, int w, int h) {
  int r, wid;
  TWindow *window;
  // assert(td->avail_wids.size > 0);

  r = wid_cb_pop(&td->avail_wids, &wid);
  td->active_windows[wid] = 1;
  window = &td->windows[wid];
  twindow_init(window, x, y, w, h);
  tdisp_set_active_window(td, wid);
  // assert(wid >= 0 && wid <= MAX_WINDOWS);
  tdisp_draw_window_outline(td, wid);
  tdisp_set_cursor(td, x+1, y+1);
}

void tdisp_clear_window(TDisplay *td, int wid) {}

// resets the cursor to the focused window's cursor position
void tdisp_reset_cursor(TDisplay *td) {
  TWindow *window;
  window = td->focused_window;
  tdisp_set_cursor(td, window->cur.x, window->cur.y);
}

// writes a character to the active window
void tdisp_writec(TDisplay *td, char c) {
  tdisp_reset_cursor(td);
  tdisp_cb_push(&td->buffer, c);
}
