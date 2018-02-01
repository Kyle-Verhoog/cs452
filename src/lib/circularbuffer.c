#include <circularbuffer.h>

void init_circularBuffer(CircularBuffer *cb) {
  cb->buffer_start = 0;
  cb->buffer_end = 0;
  cb->size = 0;
}

int push_circularBuffer(CircularBuffer *cb, unsigned int val) {
  if(cb->size < CIRCULAR_BUFFER_SIZE) {
    cb->buffer[cb->buffer_end] = val;
    cb->buffer_end = (cb->buffer_end + 1) % CIRCULAR_BUFFER_SIZE;
    cb->size++;
    return ECB_NONE;
  }

  return ECB_FULL;
}

unsigned int top_circularBuffer(CircularBuffer *cb) {
  return cb->buffer[cb->buffer_start];
}

int pop_circularBuffer(CircularBuffer *cb) {
  if(cb->size > 0) {
    cb->buffer_start = (cb->buffer_start + 1) % CIRCULAR_BUFFER_SIZE;
    cb->size--;
    return ECB_NONE;
  }

  return ECB_EMPTY;
}

