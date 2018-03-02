#include <test/terminal.h>

void tdisp_print(TDisplay *td) {
  char c;
  // printf("%d", td->buffer.size);
  while (td->buffer.size > 0) {
    tdisp_cb_pop(&td->buffer, &c);
    printf("%c", c);
  }
}

void term_init() {
  TDisplay td;
  tdisp_init(&td);
  assert(td.num_active_windows == 0);
}

void term_add_window() {
  TDisplay td;
  tdisp_init(&td);

  tdisp_add_window(&td, 50, 80, 25, 25);
  /// tdisp_print(&td);
  assert(td.windows[0].cur.x == 51);
  assert(td.windows[0].cur.y == 81);
  assert(td.windows[0].w == 25);
  assert(td.windows[0].h == 25);
}

void term_move_cursor() {
  TDisplay td;
  tdisp_init(&td);

  tdisp_add_window(&td, 50, 80, 25, 25);
  // assert
}

void terminal_tests() {
  term_init();
  term_add_window();
}
