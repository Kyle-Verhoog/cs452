#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#define ECB_NONE 0
#define ECB_FULL 1
#define ECB_EMPTY 2

#include <types.h>

typedef struct CircularBuffer {
  uint32_t *buffer;
  unsigned int buffer_start;
  unsigned int buffer_end;

  unsigned int size;
  unsigned int max_size;

} CircularBuffer;

void init_circularBuffer(CircularBuffer *cb, uint32_t *buffer, unsigned int max_size);
int push_circularBuffer(CircularBuffer *buffer, unsigned int val);
unsigned int top_circularBuffer(CircularBuffer *buffer);
int pop_circularBuffer(CircularBuffer *buffer);

#endif /* CIRCULAR_BUFFER_H */
