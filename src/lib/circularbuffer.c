#include <circularbuffer.h>

void init_circularBuffer(CircularBuffer *cb, uint32_t *buffer, unsigned int max_size) {
  cb->buffer = buffer;
  cb->buffer_start = 0;
  cb->buffer_end = 0;
  cb->size = 0;
  cb->max_size = max_size;
}

int push_circularBuffer(CircularBuffer *cb, unsigned int val) {
  if(cb->size < cb->max_size) {
    cb->buffer[cb->buffer_end] = val;
    cb->buffer_end = (cb->buffer_end + 1) % cb->max_size;
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
    cb->buffer_start = (cb->buffer_start + 1) % cb->max_size;
    cb->size--;
    return ECB_NONE;
  }

  return ECB_EMPTY;
}

