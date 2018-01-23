#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#define CIRCULAR_BUFFER_SIZE 256

typedef struct CircularBuffer{
	
	unsigned int buffer[CIRCULAR_BUFFER_SIZE];
	unsigned int buffer_start;
	unsigned int buffer_end;

} CircularBuffer;

void init_circularBuffer(CircularBuffer *buffer);

#endif /* CIRCULAR_BUFFER_H */
