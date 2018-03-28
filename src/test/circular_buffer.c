#include <test/circular_buffer.h>

EXT_CIRCULAR_BUFFER_DEC(int_cb, int, 5);
EXT_CIRCULAR_BUFFER_DEF(int_cb, int, 5);

#define SIZE 5
int int_cb_get2(int_cb *cb, int i, int *ret) {
  int r, u, v, start, size;
  start = cb->start;
  size = cb->size;
  if (cb->size <= 0)
    return CB_E_EMPTY;

  u = start + size >= SIZE ? SIZE - start : size;
  v = start + size >= SIZE ? cb->end : 0;

  r = start + u;
  i = i + start;

  if (i < r) {
    *ret = cb->buf[i];
    return CB_E_NONE;
  }

  i = i - r;

  if (i < v) {
    *ret = cb->buf[i];
    return CB_E_NONE;
  }

  return CB_E_DNE;
}

static void int_cb_print(int_cb *cb) {
  int i, u, v, start, size;

  start = cb->start;
  size = cb->size;
  u = start + (start + size >= SIZE ? SIZE - start : size);
  v = start + size >= SIZE ? cb->end : 0;
  printf("buf(%d, %d, %d): [", cb->start, cb->size, cb->end);
  for (i = 0; i < 5; ++i) {
    printf("%d, ", cb->buf[i]);
  }
  printf("\b\b]\n");

  printf("buf(%d, %d, %d): [", cb->start, cb->size, cb->end);
  for (i = start; i < u; ++i) {
    printf("%d, ", cb->buf[i]);
  }
  for (i = 0; i < v; ++i) {
    printf("%d, ", cb->buf[i]);
  }
  printf("\b\b]\n");
}

/*
int int_cb_rem(int_cb *cb, int val) {
  int i, u, v, d, j, start, size;
  start = cb->start;
  size = cb->size;
  if (cb->size <= 0)
    return CB_E_EMPTY;
  u = start + (start + size >= SIZE ? SIZE - start : size);
  v = start + size >= SIZE ? cb->end : 0;
  // printf("u %d, v %d\n", u, v);
  i = start;
  j = 0;
  for (; i < u; ++i) {
    if (cb->buf[i] == val)
      goto SHIFT;
  }
  for (; j < v; ++j) {
    if (cb->buf[j] == val)
      goto SHIFT;
  }
  return CB_E_DNE;

SHIFT:
  for (; i < u-1; ++i) {
    cb->buf[i] = cb->buf[i+1];
    // printf("%d ", cb->buf[j]);
  }
  if (i == u-1 && j < v-1) {
    cb->buf[u-1] = cb->buf[0];
  }
  for (; j < v-1; ++j) {
    cb->buf[j] = cb->buf[j+1];
  }
  cb->end = (cb->end - 1 + SIZE) % SIZE;
  cb->size--;
  return CB_E_NONE;
}
*/

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

static void int_cbuf_pop_end() {
  int r;
  int_cb cb;
  int_cb_init(&cb);
  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);

  int i;
  r = int_cb_pop_end(&cb, &i);
  assert(i == 9);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 8);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 7);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 6);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 5);
  r = int_cb_pop_end(&cb, &i);
  assert(r == CB_E_EMPTY);

  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);

  r = int_cb_pop(&cb, &i);
  assert(i == 5);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 9);
  r = int_cb_pop(&cb, &i);
  assert(i == 6);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 8);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 7);

  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);

  r = int_cb_pop(&cb, &i);
  assert(i == 5);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 9);
  r = int_cb_pop(&cb, &i);
  assert(i == 6);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 8);
  r = int_cb_pop_end(&cb, &i);
  assert(i == 7);
}

static void int_cbuf_rem() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);

  // fill the buffer completely
  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);

  // test removing the first element
  assert(cb.size == 5);
  r = int_cb_rem(&cb, 5);
  assert(r == 0);
  assert(cb.size == 4);

  r = int_cb_get(&cb, 0, &ret);
  assert(ret == 6);
  int_cb_get(&cb, 1, &ret);
  assert(ret == 7);
  int_cb_get(&cb, 2, &ret);
  assert(ret == 8);
  int_cb_get(&cb, 3, &ret);
  assert(ret == 9);


  // test removing a non-existent element
  r = int_cb_rem(&cb, -1);
  assert(r == CB_E_DNE);
  assert(cb.size == 4);


  // test removing the last element
  r = int_cb_rem(&cb, 9);
  assert(r == 0);
  assert(cb.size == 3);
  int_cb_get(&cb, 0, &ret);
  assert(ret == 6);
  int_cb_get(&cb, 1, &ret);
  assert(ret == 7);
  r = int_cb_get(&cb, 2, &ret);
  assert(ret == 8);
  r = int_cb_get(&cb, 3, &ret);
  assert(r == CB_E_DNE);


  // test with an buffer that wraps around the array

  // empty out buffer, start should be near the end of the array
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 6);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 7);
  r = int_cb_pop(&cb, &ret);
  assert(r == 0);
  assert(ret == 8);

  // confirm buffer empty, start near end of array
  assert(cb.size == 0);
  assert(cb.start == 3);

  // fill the buffer completely
  r = int_cb_push(&cb, 5); assert(r == 0);
  r = int_cb_push(&cb, 6); assert(r == 0);
  r = int_cb_push(&cb, 7); assert(r == 0);
  r = int_cb_push(&cb, 8); assert(r == 0);
  r = int_cb_push(&cb, 9); assert(r == 0);

  assert(cb.size == 5);
  assert(cb.start == 3);
  assert(cb.end == 3);

  // remove the start
  r = int_cb_rem(&cb, 5); assert(r == 0);
  assert(cb.size == 4);

  r = int_cb_get(&cb, 0, &ret); assert(r == 0);
  assert(ret == 6);
  r = int_cb_get(&cb, 1, &ret); assert(r == 0);
  assert(ret == 7);
  r = int_cb_get(&cb, 2, &ret); assert(r == 0);
  assert(ret == 8);
  r = int_cb_get(&cb, 3, &ret); assert(r == 0);
  assert(ret == 9);

  // remove a middle element
  r = int_cb_rem(&cb, 7); assert(r == 0);
  r = int_cb_get(&cb, 0, &ret); assert(r == 0);
  assert(ret == 6);
  r = int_cb_get(&cb, 1, &ret); assert(r == 0);
  assert(ret == 8);
  r = int_cb_get(&cb, 2, &ret); assert(r == 0);
  assert(ret == 9);

  // remove the last element
  r = int_cb_rem(&cb, 9); assert(r == 0);
  r = int_cb_rem(&cb, 9); assert(r == CB_E_DNE);

  // push on a few more elements and remove them for good measure

  r = int_cb_push(&cb, 11); assert(r == 0);
  r = int_cb_push(&cb, 12); assert(r == 0);
  r = int_cb_push(&cb, 13); assert(r == 0);

  r = int_cb_rem(&cb, 11); assert(r == 0);
  r = int_cb_get(&cb, 0, &ret); assert(r == 0);
  assert(ret == 6);
  r = int_cb_get(&cb, 1, &ret); assert(r == 0);
  assert(ret == 8);
  r = int_cb_get(&cb, 2, &ret); assert(r == 0);
  assert(ret == 12);
  r = int_cb_get(&cb, 3, &ret); assert(r == 0);
  assert(ret == 13);
}

// test removing when wrapping around the buffer
static void int_cbuf_rem_wrap() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);

  // fill the buffer completely
  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);

  // move the start of the buffer up
  r = int_cb_pop(&cb, &ret);
  r = int_cb_pop(&cb, &ret);
  r = int_cb_pop(&cb, &ret);
  r = int_cb_push(&cb, 10);
  r = int_cb_push(&cb, 11);
  r = int_cb_push(&cb, 12);

  // repeatedly remove the last element
  r = int_cb_rem(&cb, 12); assert(r == 0);
  assert(!int_cb_get(&cb, 0, &ret));
  assert(ret == 8);
  assert(!int_cb_get(&cb, 1, &ret));
  assert(ret == 9);
  assert(!int_cb_get(&cb, 2, &ret));
  assert(ret == 10);
  assert(!int_cb_get(&cb, 3, &ret));
  assert(ret == 11);
  assert(int_cb_get(&cb, 4, &ret));

  r = int_cb_rem(&cb, 11); assert(r == 0);
  assert(!int_cb_get(&cb, 0, &ret));
  assert(ret == 8);
  assert(!int_cb_get(&cb, 1, &ret));
  assert(ret == 9);
  assert(!int_cb_get(&cb, 2, &ret));
  assert(ret == 10);
  assert(int_cb_get(&cb, 3, &ret));

  r = int_cb_rem(&cb, 10); assert(r == 0);
  assert(!int_cb_get(&cb, 0, &ret));
  assert(ret == 8);
  assert(!int_cb_get(&cb, 1, &ret));
  assert(ret == 9);
  assert(int_cb_get(&cb, 2, &ret));

  r = int_cb_rem(&cb, 9); assert(r == 0);
  assert(!int_cb_get(&cb, 0, &ret));
  assert(ret == 8);
  assert(int_cb_get(&cb, 1, &ret));

  r = int_cb_rem(&cb, 8); assert(r == 0);
  assert(int_cb_get(&cb, 0, &ret));

  r = int_cb_push(&cb, 12); assert(r == 0);
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

static void int_cbuf_get2() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);

  r = int_cb_get2(&cb, 0, &ret);
  assert(r == CB_E_EMPTY);

  r = int_cb_push(&cb, 5);
  r = int_cb_get2(&cb, 0, &ret);
  assert(ret == 5);

  r = int_cb_push(&cb, 6);
  r = int_cb_get2(&cb, 0, &ret);
  assert(ret == 5);
  r = int_cb_get2(&cb, 1, &ret);
  assert(ret == 6);

  r = int_cb_push(&cb, 7);
  r = int_cb_get2(&cb, 0, &ret);
  assert(ret == 5);
  r = int_cb_get2(&cb, 1, &ret);
  assert(ret == 6);
  r = int_cb_get2(&cb, 2, &ret);
  assert(ret == 7);

  r = int_cb_push(&cb, 8);
  r = int_cb_get2(&cb, 0, &ret);
  assert(ret == 5);
  r = int_cb_get2(&cb, 1, &ret);
  assert(ret == 6);
  r = int_cb_get2(&cb, 2, &ret);
  assert(ret == 7);
  r = int_cb_get2(&cb, 3, &ret);
  assert(ret == 8);

  r = int_cb_push(&cb, 9);
  r = int_cb_get2(&cb, 0, &ret);
  assert(ret == 5);
  r = int_cb_get2(&cb, 1, &ret);
  assert(ret == 6);
  assert(r == 0);
  r = int_cb_get2(&cb, 2, &ret);
  assert(ret == 7);
  assert(r == 0);
  r = int_cb_get2(&cb, 3, &ret);
  assert(ret == 8);
  assert(r == 0);
  r = int_cb_get2(&cb, 4, &ret);
  assert(ret == 9);

  r = int_cb_pop(&cb, &ret);
  r = int_cb_get2(&cb, 0, &ret);
  assert(ret == 6);
  assert(r == 0);
  r = int_cb_get2(&cb, 1, &ret);
  assert(ret == 7);
  assert(r == 0);
  r = int_cb_get2(&cb, 2, &ret);
  assert(ret == 8);
  assert(r == 0);
  r = int_cb_get2(&cb, 3, &ret);
  assert(ret == 9);
  assert(r == 0);

  r = int_cb_push(&cb, 10);
  r = int_cb_get2(&cb, 0, &ret);
  assert(ret == 6);
  assert(r == 0);
  r = int_cb_get2(&cb, 1, &ret);
  assert(ret == 7);
  assert(r == 0);
  r = int_cb_get2(&cb, 2, &ret);
  assert(ret == 8);
  assert(r == 0);
  r = int_cb_get2(&cb, 3, &ret);
  assert(ret == 9);
  assert(r == 0);
  r = int_cb_get2(&cb, 4, &ret);
  assert(ret == 10);
  assert(r == 0);

  r = int_cb_get2(&cb, 5, &ret);
  assert(r == CB_E_DNE);
}

static void int_cbuf_get() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);

  r = int_cb_get(&cb, 0, &ret);
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
  assert(r == CB_E_DNE);
}

static void int_cbuf_get_2() {
  int r, ret;
  int_cb cb;
  int_cb_init(&cb);
  r = int_cb_push(&cb, 5);
  r = int_cb_push(&cb, 6);
  r = int_cb_push(&cb, 7);
  r = int_cb_push(&cb, 8);
  r = int_cb_push(&cb, 9);

  r = int_cb_get(&cb, 0, &ret);
  assert(r == 0);
  assert(ret == 5);

  r = int_cb_get(&cb, 1, &ret);
  assert(r == 0);
  assert(ret == 6);

  r = int_cb_get(&cb, 2, &ret);
  assert(r == 0);
  assert(ret == 7);

  r = int_cb_get(&cb, 3, &ret);
  assert(r == 0);
  assert(ret == 8);

  r = int_cb_get(&cb, 4, &ret);
  assert(r == 0);
  assert(ret == 9);
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
  int_cbuf_get2();
  int_cbuf_get_2();
  int_cbuf_reuse();
  int_cbuf_pop_end();
  int_cbuf_rem();
  int_cbuf_rem_wrap();
  char_cbuf();
  char_cbuf_test();
  ptr_cb_test();
}
