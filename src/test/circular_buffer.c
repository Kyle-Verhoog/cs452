#include <test/circular_buffer.h>

CIRCULAR_BUFFER_DEC(int_cb, int, 5);
CIRCULAR_BUFFER_DEF(int_cb, int, 5);

static void int_cbuf() {
  int_cb cb;
  int_cb_init(&cb);
  assert(cb.size == 0);
}

static void int_cbuf_push_1() {
  int r;
  int_cb cb;
  int_cb_init(&cb);
  r = int_cb_push(&cb, 5);
  assert(r == 0);
  assert(cb.size == 1);
}

static void int_cbuf_top_1() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);
  r = int_cb_push(&cb, 5);
  assert(cb.size == 1);
  r = int_cb_top(&cb, &ret);
  assert(r == 0);
  assert(ret == 5);
  assert(cb.size == 1);
  r = int_cb_top(&cb, &ret);
  assert(r == 0);
  assert(ret == 5);
}

static void int_cbuf_pop_1() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);
  r = int_cb_push(&cb, 5);
  assert(r == 0);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 5);
}

static void int_cbuf_push_2() {
  int r;
  int_cb cb;
  int_cb_init(&cb);
  r = int_cb_push(&cb, 5);
  assert(r == 0);
  r = int_cb_push(&cb, 6);
  assert(r == 0);
  r = int_cb_push(&cb, 7);
  assert(r == 0);
  r = int_cb_push(&cb, 8);
  assert(r == 0);
  r = int_cb_push(&cb, 9);
  assert(r == 0);
  assert(cb.size == 5);
}


static void int_cbuf_pop_2() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);
  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);

  // test the buffer being full
  r = int_cb_push(&cb, 10);
  assert(r == CB_E_FULL);
  r = int_cb_push(&cb, 10);
  assert(r == CB_E_FULL);

  // test all the values
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 5);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 6);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 7);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 8);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 9);

  // test the buffer being empty
  r = int_cb_pop(&cb, &ret);
  assert(r == CB_E_EMPTY);
}

static void int_cbuf_get() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);

  r = int_cb_get(&cb, 0, &ret);
  // printf("%d\n", (cb.end-1 + 5) % 5);
  assert(r == CB_E_DNE);

  r = int_cb_push(&cb, 5);
  r = int_cb_get(&cb, 0, &ret);
  assert(ret == 5);

  r = int_cb_push(&cb, 6);
  r = int_cb_get(&cb, 0, &ret);
  assert(ret == 5);
  r = int_cb_get(&cb, 1, &ret);
  assert(ret == 6);

  r = int_cb_push(&cb, 7);
  r = int_cb_get(&cb, 0, &ret);
  assert(ret == 5);
  r = int_cb_get(&cb, 1, &ret);
  assert(ret == 6);
  r = int_cb_get(&cb, 2, &ret);
  assert(ret == 7);

  r = int_cb_push(&cb, 8);
  r = int_cb_get(&cb, 0, &ret);
  assert(ret == 5);
  r = int_cb_get(&cb, 1, &ret);
  assert(ret == 6);
  r = int_cb_get(&cb, 2, &ret);
  assert(ret == 7);
  r = int_cb_get(&cb, 3, &ret);
  assert(ret == 8);

  r = int_cb_push(&cb, 9);
  r = int_cb_get(&cb, 0, &ret);
  // printf("%d\n", (cb.end-1 + 5) % 5);
  assert(ret == 5);
  r = int_cb_get(&cb, 1, &ret);
  assert(ret == 6);
  assert(r == 0);
  r = int_cb_get(&cb, 2, &ret);
  assert(ret == 7);
  assert(r == 0);
  r = int_cb_get(&cb, 3, &ret);
  assert(ret == 8);
  assert(r == 0);
  r = int_cb_get(&cb, 4, &ret);
  assert(ret == 9);

  r = int_cb_pop(&cb, &ret);
  r = int_cb_get(&cb, 0, &ret);
  assert(ret == 6);
  assert(r == 0);
  r = int_cb_get(&cb, 1, &ret);
  assert(ret == 7);
  assert(r == 0);
  r = int_cb_get(&cb, 2, &ret);
  assert(ret == 8);
  assert(r == 0);
  r = int_cb_get(&cb, 3, &ret);
  assert(ret == 9);
  assert(r == 0);

  r = int_cb_push(&cb, 10);
  r = int_cb_get(&cb, 0, &ret);
  assert(ret == 6);
  assert(r == 0);
  r = int_cb_get(&cb, 1, &ret);
  assert(ret == 7);
  assert(r == 0);
  r = int_cb_get(&cb, 2, &ret);
  assert(ret == 8);
  assert(r == 0);
  r = int_cb_get(&cb, 3, &ret);
  assert(ret == 9);
  assert(r == 0);
  r = int_cb_get(&cb, 4, &ret);
  assert(ret == 10);
  assert(r == 0);

  r = int_cb_get(&cb, 5, &ret);
  assert(r == CB_E_OOB);
}

static void int_cbuf_reuse() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);

  // fill the buffer
  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);

  // empty the buffer
  r = int_cb_pop(&cb, &ret);
  r = int_cb_pop(&cb, &ret);
  r = int_cb_pop(&cb, &ret);
  r = int_cb_pop(&cb, &ret);
  r = int_cb_pop(&cb, &ret);

  // fill the buffer (again)
  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);
  assert(r == 0);

  // test all the values
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 5);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 6);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 7);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 8);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 9);

  // test the buffer being empty
  r = int_cb_pop(&cb, &ret);
  assert(r == CB_E_EMPTY);
}


CIRCULAR_BUFFER_DEC(tchar_cb, char, 10);
CIRCULAR_BUFFER_DEF(tchar_cb, char, 10);

void char_cbuf() {
  tchar_cb cb;
  tchar_cb_init(&cb);
  assert(cb.size == 0);
}

void char_cbuf_test() {
  int r;
  char c;
  tchar_cb cb;
  tchar_cb_init(&cb);

  r = tchar_cb_push(&cb, 't');
  assert(r == 0);
  r = tchar_cb_push(&cb, 'e');
  assert(r == 0);
  r = tchar_cb_push(&cb, 's');
  assert(r == 0);
  r = tchar_cb_push(&cb, 't');
  assert(r == 0);
  r = tchar_cb_push(&cb, 'i');
  assert(r == 0);
  r = tchar_cb_push(&cb, 'n');
  assert(r == 0);
  r = tchar_cb_push(&cb, 'g');
  assert(r == 0);
  r = tchar_cb_push(&cb, '.');
  assert(r == 0);
  r = tchar_cb_push(&cb, '.');
  assert(r == 0);
  r = tchar_cb_push(&cb, '\0');
  assert(r == 0);
  r = tchar_cb_push(&cb, '\0');
  assert(cb.size == 10);
  assert(r == CB_E_FULL);

  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == 't');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == 'e');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == 's');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == 't');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == 'i');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == 'n');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == 'g');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == '.');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == '.');
  r = tchar_cb_pop(&cb, &c);
  assert(r == 0);
  assert(c == '\0');
  r = tchar_cb_pop(&cb, &c);
  assert(r == CB_E_EMPTY);
  assert(cb.size == 0);
}


CIRCULAR_BUFFER_DEC(ptr_cb, int*, 3);
CIRCULAR_BUFFER_DEF(ptr_cb, int*, 3);

void ptr_cb_test() {
  int r;
  ptr_cb cb;
  ptr_cb_init(&cb);
  assert(cb.size == 0);
  int *p = &r;
  r = ptr_cb_push(&cb, p);
  assert(r == 0);
  int *ret;
  r = ptr_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == p);
}

void cbuf_tests() {
  int_cbuf();
  int_cbuf_push_1();
  int_cbuf_top_1();
  int_cbuf_pop_1();
  int_cbuf_push_2();
  int_cbuf_pop_2();
  int_cbuf_get();
  int_cbuf_reuse();
  char_cbuf();
  char_cbuf_test();
  ptr_cb_test();
}
