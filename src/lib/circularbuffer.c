#include <circularbuffer.h>

void init_circularBuffer(CircularBuffer *cb) {
  cb->buffer_start = 0;
  cb->buffer_end = 0;
}

void push_circularBuffer(CircularBuffer *cb, unsigned int val) {
  cb->buffer[cb->buffer_end] = val;
  cb->buffer_end = (cb->buffer_end + 1) % CIRCULAR_BUFFER_SIZE;
}

unsigned int top_circularBuffer(CircularBuffer *cb) {
  return cb->buffer[cb->buffer_start];
}

void pop_circularBuffer(CircularBuffer *cb) {
  cb->buffer_start = (cb->buffer_start + 1) % CIRCULAR_BUFFER_SIZE;
}

