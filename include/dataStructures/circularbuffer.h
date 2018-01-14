#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H


#define CIRCULAR_BUFFER_SIZE 256

typedef struct CircularBuffer{
	
	char buffer[CIRCULAR_BUFFER_SIZE];
	unsigned int buffer_start;
	unsigned int buffer_end;

} CircularBuffer;

init_circularBuffer(CircularBuffer *buffer);

#endif /* CIRCULAR_BUFFER_H */