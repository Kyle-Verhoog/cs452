#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#define CIRCULAR_BUFFER_SIZE 256
#define ECB_NONE 0
#define ECB_FULL 1
#define ECB_EMPTY 2

typedef struct CircularBuffer {

  unsigned int buffer[CIRCULAR_BUFFER_SIZE];
  unsigned int buffer_start;
  unsigned int buffer_end;

  unsigned int size;

} CircularBuffer;

void init_circularBuffer(CircularBuffer *buffer);
int push_circularBuffer(CircularBuffer *buffer, unsigned int val);
unsigned int top_circularBuffer(CircularBuffer *buffer);
int pop_circularBuffer(CircularBuffer *buffer);

#endif /* CIRCULAR_BUFFER_H */
