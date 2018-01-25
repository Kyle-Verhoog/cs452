#include <assert.h>
#include <test/circularbuffer.h>

void cb_sanity() {
  CircularBuffer cb;
  init_circularBuffer(&cb);

  int i;
  for(i = 0; i < CIRCULAR_BUFFER_SIZE; i++) {
    push_circularBuffer(&cb, i);
  }

  for(i = CIRCULAR_BUFFER_SIZE - 1; i >= 0; i--) {
    int n = top_circularBuffer(&cb);
    assert(CIRCULAR_BUFFER_SIZE-1 - i == n);
    pop_circularBuffer(&cb);
  }
}

void circular_buffer_tests() {
  cb_sanity();
  printf("✓ CircularBuffer\n");
}
