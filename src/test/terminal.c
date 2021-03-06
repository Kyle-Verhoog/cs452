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
  int bsize;

  bsize = 0;
  tdisp_init(&td);
  assert(td.tdcur.x == -1);
  assert(td.tdcur.y == -1);

  tdisp_add_window(&td, 50, 80, 10, 20, 0);
  // tdisp_print(&td);
  // printf("%d\n", td.tdcur.x);
  assert(td.tdcur.x == 51);
  assert(td.tdcur.y == 81);
  // printf("%d\n",td.windows[0].cur.x);
  assert(td.windows[0].cur.x == 1);
  assert(td.windows[0].cur.y == 1);
  assert(td.windows[0].offsetx == 50);
  assert(td.windows[0].offsety == 80);
  assert(td.windows[0].w == 10);
  assert(td.windows[0].h == 20);
  // printf("%d\n", td.buffer.size);
  assert(td.buffer.size == 501);
  bsize += td.buffer.size;

  tdisp_writec(&td, 'a');
  assert(td.tdcur.x == 50 + 2);
  assert(td.tdcur.y == 80 + 1);
  assert(td.windows[0].cur.x == 2);
  assert(td.windows[0].cur.y == 1);
  assert(td.windows[0].offsetx == 50);
  assert(td.windows[0].offsety == 80);
  // printf("%d\n", td.windows[0].w);
  assert(td.windows[0].w == 10);
  assert(td.windows[0].h == 20);
  // printf("%d\n", td.buffer.size);
  assert(td.buffer.size == bsize + 1);
  bsize += 1;
  assert(td.focused_window == &td.windows[0]);

  tdisp_writec(&td, 'a');
  assert(td.tdcur.x == 50 + 3);
  assert(td.tdcur.y == 80 + 1);
  assert(td.windows[0].cur.x == 3);
  assert(td.windows[0].cur.y == 1);
  assert(td.windows[0].offsetx == 50);
  assert(td.windows[0].offsety == 80);
  // printf("%d\n", td.windows[0].w);
  assert(td.windows[0].w == 10);
  assert(td.windows[0].h == 20);
  // printf("%d\n", td.buffer.size);
  assert(td.buffer.size == bsize + 1);
  bsize += 1;

  tdisp_writec(&td, BACKSPACE);
  assert(td.tdcur.x == 50 + 2);
  assert(td.tdcur.y == 80 + 1);
  assert(td.windows[0].cur.x == 2);
  assert(td.windows[0].cur.y == 1);
  assert(td.windows[0].offsetx == 50);
  assert(td.windows[0].offsety == 80);
  // printf("%d\n", td.windows[0].w);
  assert(td.windows[0].w == 10);
  assert(td.windows[0].h == 20);
  // printf("%d\n", td.buffer.size);
  assert(td.buffer.size == bsize + 1);
  bsize += 1;
}

void term_focus() {
  TDisplay td;
  tdisp_init(&td);
  tdisp_add_window(&td, 1, 1, 10, 5, 0);
  tdisp_add_window(&td, 1, 6, 10, 5, 1);
  assert(td.focused_window == &td.windows[1]);
  // printf("%d\n", td.tdcur);
  assert(td.tdcur.x == 2);
  assert(td.tdcur.y == 7);
}

void term_writec() {
  TDisplay td;
  int bsize;
  bsize = 0;
  tdisp_init(&td);
  tdisp_add_window(&td, 1, 1, 5 , 5, 0);
  assert(td.buffer.size == 118);
  bsize += td.buffer.size;
  tdisp_writec(&td, 'a');
  assert(td.buffer.size == bsize + 1);
  bsize += 1;
  tdisp_writec(&td, 'a');
  assert(td.buffer.size == bsize + 1);
  bsize += 1;
  assert(td.tdcur.x == 4);
  assert(td.tdcur.y == 2);
  assert(td.windows[0].cur.x == 3);
  assert(td.windows[0].cur.y == 1);

  tdisp_writec(&td, 'a');
  //printf("%d\n", td.windows[0].cur.x);
  // printf("%d\n", td.tdcur.x);
  assert(td.tdcur.x == 5);
  // printf("%d\n", td.tdcur.y);
  assert(td.tdcur.y == 2);
  // printf("%d\n", td.windows[0].cur.x);
  assert(td.buffer.size == bsize + 1);
  bsize += 1;
  assert(td.windows[0].cur.x == 4);
  assert(td.windows[0].cur.y == 1);

  tdisp_writec(&td, 'a');
  assert(td.buffer.size == bsize + 1);
  bsize += 1;
  assert(td.tdcur.x == 6);
  assert(td.tdcur.y == 2);
  assert(td.windows[0].cur.x == 5);
  assert(td.windows[0].cur.y == 1);

  // printf("%d\n", td.buffer.size);
  tdisp_writec(&td, '\v');
  // printf("%d\n", td.buffer.size);
  assert(td.buffer.size == bsize + 6);
  bsize += 6;
  assert(td.tdcur.x == 2);
  assert(td.tdcur.y == 2);
  assert(td.windows[0].cur.x == 1);
  assert(td.windows[0].cur.y == 1);

  tdisp_writec(&td, '\n');
  // printf("%d\n", td.buffer.size);
  assert(td.buffer.size == bsize + 6);
  bsize += 6;
  assert(td.tdcur.x == 2);
  assert(td.tdcur.y == 3);
  assert(td.windows[0].cur.x == 1);
  assert(td.windows[0].cur.y == 2);
}

void term_move_cursor() {
  TDisplay td;
  tdisp_init(&td);

  tdisp_add_window(&td, 50, 80, 25, 25, 0);
}

void terminal_tests() {
  term_init();
  term_add_window();
  term_focus();
  term_writec();
  term_move_cursor();
}
