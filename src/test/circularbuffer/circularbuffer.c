#include <test/circularbuffer.h>

#define BUFFER_SIZE 256

void cb_sanity() {
  int buffer[BUFFER_SIZE];
  CircularBuffer cb;
  init_circularBuffer(&cb, buffer, BUFFER_SIZE);

  int i;
  for(i = 0; i < BUFF_SIZE; i++) {
    push_circularBuffer(&cb, i);
  }

  for(i = BUFFER_SIZE - 1; i >= 0; i--) {
    int n = top_circularBuffer(&cb);
    assert(BUFFER_SIZE-1 - i == n);
    pop_circularBuffer(&cb);
  }
}

void circular_buffer_tests() {
  cb_sanity();
}
