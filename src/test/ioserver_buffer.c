#include <test/ioserver_buffer.h>

void iobuf_init() {
  io_cb cb;
  io_cb_init(&cb);
  assert(cb.size == 0);
}

void iobuf_push() {
  io_cb cb;
  io_cb_init(&cb);
  int r;

  iobuf_item i;
  i.c = 'a';
  i.btid = 9;
  r = io_cb_push(&cb, i);
  assert(r == 0);
  assert(cb.size == 1);
  r = io_cb_push(&cb, i);
  assert(r == 0);
  r = io_cb_push(&cb, i);
  assert(r == 0);
  r = io_cb_push(&cb, i);
  assert(r == 0);
  r = io_cb_push(&cb, i);
  assert(r == 0);
  assert(cb.size == 5);
}

void iobuf_pop() {
  io_cb cb;
  io_cb_init(&cb);
  int r;

  iobuf_item i;
  i.c = 'a';
  i.btid = 9;
  r = io_cb_push(&cb, i);
  assert(r == 0);
  assert(cb.size == 1);

  iobuf_item j;
  r = io_cb_pop(&cb, &j);
  assert(r == 0);
  assert(cb.size == 0);
  assert(j.c == i.c);
  assert(j.btid == i.btid);

  i.c = '1';
  i.btid = 23232345;
  r = io_cb_push(&cb, i);
  assert(r == 0);
  i.c = '2';
  i.btid = 43;
  r = io_cb_push(&cb, i);
  assert(r == 0);
  i.c = '3';
  i.btid = 3242;
  r = io_cb_push(&cb, i);
  assert(r == 0);

  r = io_cb_pop(&cb, &j);
  assert(r == 0);
  assert(j.c == '1');
  assert(j.btid == 23232345);

  r = io_cb_pop(&cb, &j);
  assert(r == 0);
  assert(j.c == '2');
  assert(j.btid == 43);

  r = io_cb_pop(&cb, &j);
  assert(r == 0);
  assert(j.c == '3');
  assert(j.btid == 3242);
}

void ioserver_buffer_tests() {
  iobuf_init();
  iobuf_push();
  iobuf_pop();
}
